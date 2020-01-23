#include "stdafx.h"

// ETRI
bool loadAvatarDump(GScene *pScene, const char *fileName)
{
	FILE *fp;
	fopen_s(&fp, fileName, "r");
	if (fp == NULL)
		return false;

	std::string RootMeshName = get_file_name(fileName);
	GMesh *pRootMesh = new GMesh(RootMeshName, GTransf(), FACE_TRI);
	GMaterial mtl = ::get_material(0.5, 0.5, 0.5);
	pRootMesh->AddMtl(mtl);
	pRootMesh->SetMtl(mtl.Name);
	pScene->AddMesh(pRootMesh);

	// "boneHierarchy"와 "boneBindPose" 태그가 있는 경우,
	std::vector<GMesh *> BoneMeshList;
	std::vector<std::string> BoneMeshNameList;

	// 줄 단위로 읽어온 데이터를 분석하기 위한 변수
	char buffer[256];
	char delimiter[] = " \t\n";
	char *tokenNext = NULL;

	// 첫 번째 파싱을 수행한다.
	while (fgets(buffer, 256, fp))
	{
		// 태그를 읽고, "#" 이나 "version" 으로 시작하는 줄은 건너뜀
		char *tagName = strtok_s(buffer, delimiter, &tokenNext);
		char *tagVal = strtok_s(NULL, delimiter, &tokenNext);
		if (!strcmp(tagName, "#") || !strcmp(tagName, "version"))
			continue;

		// 태그가 "boneHierarchy"인 경우,
		if (!strcmp(tagName, "boneHierarchy"))
		{
			int NumBone = tagVal ? atoi(tagVal) : 0;
			for(int i = 0; i < NumBone; ++i)
			{
				// # idx name parentName 건너 띄기
				while (fgets(buffer, 256, fp))
				{
					if(buffer[0] != '#') 
						break;
				}

				int idx;
				char BoneName[256] = {0, };
				char ParentName[256] = {0, };

				// idx name parentName
				sscanf_s(buffer, "%d%s%s", &idx, BoneName, 256, ParentName, 256);

				std::string BoneMeshName = RootMeshName + "_" + BoneName;
				std::string ParentMeshName = RootMeshName + "_" + ParentName;

				if (i == 0)
				{
					GMesh *pBoneMesh = new GMesh(BoneMeshName, GTransf(), FACE_TRI, RootMeshName);
					pBoneMesh->SemanticType = MESH_BONE;
					BoneMeshList.push_back(pBoneMesh);
					BoneMeshNameList.push_back(BoneMeshName);
				}
				else
				{
					GMesh *pBoneMesh = new GMesh(BoneMeshName, GTransf(), FACE_TRI, ParentMeshName);
					pBoneMesh->SemanticType = MESH_BONE;
					BoneMeshList.push_back(pBoneMesh);
					BoneMeshNameList.push_back(BoneMeshName);
				}
			}
		}
		else if (!strcmp(tagName, "boneBindPose"))	// "boneBindPose" 태그일 경우,
		{
			int NumBone = tagVal ? atoi(tagVal) : 0;
			for (int i = 0; i < NumBone; i++)
			{
				// # m00, m01, ..., m44		: 4x4 transform matrix, row major 건너 띄기
				while (fgets(buffer, 256, fp))
				{
					if(buffer[0] != '#') 
						break;
				}

				// Matrix bindPose;
				double bindPose[16];

				// m00, m01, ..., m44		: 4x4 transform matrix, row major
				sscanf_s(buffer, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf",
					&bindPose[0], &bindPose[1], &bindPose[2], &bindPose[3], 
					&bindPose[4], &bindPose[5], &bindPose[6], &bindPose[7], 
					&bindPose[8], &bindPose[9], &bindPose[10], &bindPose[11], 
					&bindPose[12], &bindPose[13], &bindPose[14], &bindPose[15]);

				bindPose[3] /= 10.0;
				bindPose[7] /= 10.0;
				bindPose[11] /= 10.0;

				BoneMeshList[i]->DelX = GTransf(bindPose, false);
			}

			while (!BoneMeshList.empty())
			{
				std::vector<GMesh *>::iterator it = BoneMeshList.begin();
				while(it != BoneMeshList.end())
				{
					GMesh *pBoneMesh = *it;
					GMesh *pParentBoneMesh = FindMesh(pScene->MeshList, pBoneMesh->ParentMeshName);
					if (pParentBoneMesh != NULL)
					{
						// 본 메쉬의 모델링 좌표를 계산한다.
						pBoneMesh->MC = pParentBoneMesh->MC * pBoneMesh->DelX;

						// 본 메쉬 재질
						GMaterial mtl = ::get_material(0.0, 0.5, 0.5);
						pBoneMesh->AddMtl(mtl);
						pScene->AddMesh(pBoneMesh);
						it = BoneMeshList.erase(it);
						continue;
					}
					it++;
				}
			}
			
			// 본 메쉬의 렌더링을 위한 형상을 생성한다.
			std::string RootBoneMeshName = RootMeshName + "_Root";

			GMesh *pRootBoneMesh = FindMesh(pScene->MeshList, RootBoneMeshName);
			std::vector<GMesh *> PreOrderMeshList;
			GetPreOrderMeshList(pScene, PreOrderMeshList, pRootBoneMesh);

			int NumMesh = (int)PreOrderMeshList.size();
			for (int i = 1; i < NumMesh; ++i)
			{
				GMesh *pParentBoneMesh = FindMesh(PreOrderMeshList, PreOrderMeshList[i]->ParentMeshName);

				GTransf ParentX = pParentBoneMesh->MC;
				GTransf ChildX = PreOrderMeshList[i]->MC;

				GVector3 axis = ChildX.t - ParentX.t;
				double len = norm(axis);
				double sz = len * 0.05;

				GPoint3 p0(0.0, len, 0.0);
				GPoint3 p1(-sz, 0.0, -sz);
				GPoint3 p2(-sz, 0.0, sz);
				GPoint3 p3(sz, 0.0, sz);
				GPoint3 p4(sz, 0.0, -sz);
				GPoint3 p5(0.0, len * 0.25, 0.0);
				GPoint3 p6(0.0, len * 0.5, 0.0);
				GPoint3 p7(0.0, len * 0.75, 0.0);

				GQuater q;
				q.SetFromArcBall(GVector3(0.0, 1.0, 0.0), axis);

				GTransf X(ParentX.t, q);

				p0 = inv(ParentX) * X * p0;
				p1 = inv(ParentX) * X * p1;
				p2 = inv(ParentX) * X * p2;
				p3 = inv(ParentX) * X * p3;
				p4 = inv(ParentX) * X * p4;
				p5 = inv(ParentX) * X * p5;
				p6 = inv(ParentX) * X * p6;
				p7 = inv(ParentX) * X * p7;

				int baseIdx = (int)pParentBoneMesh->Verts.size();
				pParentBoneMesh->AddVertex(GVertex(p0, GVector3()));
				pParentBoneMesh->AddVertex(GVertex(p1, GVector3()));
				pParentBoneMesh->AddVertex(GVertex(p2, GVector3()));
				pParentBoneMesh->AddVertex(GVertex(p3, GVector3()));
				pParentBoneMesh->AddVertex(GVertex(p4, GVector3()));
				pParentBoneMesh->AddVertex(GVertex(p5, GVector3()));
				pParentBoneMesh->AddVertex(GVertex(p6, GVector3()));
				pParentBoneMesh->AddVertex(GVertex(p7, GVector3()));
				pParentBoneMesh->AddFace(GFace(baseIdx + 0, baseIdx + 1, baseIdx + 2, -1, -1, -1, 0));
				pParentBoneMesh->AddFace(GFace(baseIdx + 0, baseIdx + 2, baseIdx + 3, -1, -1, -1, 0));
				pParentBoneMesh->AddFace(GFace(baseIdx + 0, baseIdx + 3, baseIdx + 4, -1, -1, -1, 0));
				pParentBoneMesh->AddFace(GFace(baseIdx + 0, baseIdx + 4, baseIdx + 1, -1, -1, -1, 0));
				pParentBoneMesh->AddFace(GFace(baseIdx + 1, baseIdx + 3, baseIdx + 2, -1, -1, -1, 0));
				pParentBoneMesh->AddFace(GFace(baseIdx + 3, baseIdx + 1, baseIdx + 4, -1, -1, -1, 0));
				pParentBoneMesh->CreateMesh();
			}
		}
	}

	// 파일의 포인터를 처음으로.
	fseek(fp, 0, SEEK_SET);

	while (fgets(buffer, 256, fp))	// 파일의 끝까지 줄 단위로 읽는다.
	{
		// 태그를 읽고, "#" 이나 "version" 으로 시작하는 줄은 건너 띔
		char *tagName = strtok_s(buffer, delimiter, &tokenNext);
		char *tagVal = strtok_s(NULL, delimiter, &tokenNext);
		if (!strcmp(tagName, "#") || !strcmp(tagName, "version"))
			continue;

		// "mesh" 또는 "graphicalMesh" 태그일 경우,
		if (!strcmp(tagName, "mesh") || !strcmp(tagName, "graphicalMesh"))
		{
			int NumMesh = tagVal ? atoi(tagVal) : 0;
			for (int MeshId = 0; MeshId < NumMesh; ++MeshId)
			{
				// 서브 메쉬 생성
				std::string SubMeshName;
				switch (MeshId)
				{
				case 0:
					SubMeshName = RootMeshName + "_skin";
					break;

				case 1:
					SubMeshName = RootMeshName + "_skirt";
					break;

				default:
					SubMeshName = RootMeshName + "_" + cast_str(MeshId);
					break;
				}
				GMesh *pSubMesh = new GMesh(SubMeshName, GTransf(), FACE_TRI, RootMeshName);
				pSubMesh->SemanticType = MESH_SKIN;

				// "#" 건너 띄기
				while (fgets(buffer, 256, fp))
				{
					if(buffer[0] != '#') 
						break;
				}
				
				// 태그를 읽고,
				tagName = strtok_s(buffer, delimiter, &tokenNext);
				tagVal = strtok_s(NULL, delimiter, &tokenNext);

				// "vertex" 태그일 경우,
				if (!strcmp(tagName, "vertex"))
				{
					int NumVert = tagVal ? atoi(tagVal) : 0;
					for (int i = 0; i < NumVert; ++i)
					{
						// # posX posY posZ normX 건너 띄기
						while (fgets(buffer, 256, fp))
						{
							if(buffer[0] != '#') 
								break;
						}

						// 정점의 정보 읽기
						float x, y, z;
						float nx, ny, nz;
						float u, v;
						int boneIdx0, boneIdx1, boneIdx2, boneIdx3;
						float boneWgt0, boneWgt1, boneWgt2, boneWgt3;
						int mappedAvatarVertIdx;
						float mappedAvatarVertexOffsetX, mappedAvatarVertexOffsetY, mappedAvatarVertexOffsetZ;
						
						// posX posY posZ normX normY normZ...
						sscanf_s(buffer, "%f%f%f%f%f%f%f%f%d%d%d%d%f%f%f%f%d%f%f%f",
								&x, &y, &z, 
								&nx, &ny, &nz, 
								&u, &v, 
								&boneIdx0, &boneIdx1, &boneIdx2, &boneIdx3, 
								&boneWgt0, &boneWgt1, &boneWgt2, &boneWgt3, 
								&mappedAvatarVertIdx, 
								&mappedAvatarVertexOffsetX, &mappedAvatarVertexOffsetY, &mappedAvatarVertexOffsetZ);
							
						x /= 10.0;
						y /= 10.0;
						z /= 10.0;
						GVertex vert(GPoint3(x, y, z), GVector3(nx, ny, nz));
						if (!BoneMeshNameList.empty())
						{
 							vert.AddRefBoneInfo(BoneMeshNameList[boneIdx0], boneWgt0);
							vert.AddRefBoneInfo(BoneMeshNameList[boneIdx1], boneWgt1);
							vert.AddRefBoneInfo(BoneMeshNameList[boneIdx2], boneWgt2);
							vert.AddRefBoneInfo(BoneMeshNameList[boneIdx3], boneWgt3);
						}
 						pSubMesh->AddVertex(vert);				
					}
				}

				// "#" 건너 띄기
				while (fgets(buffer, 256, fp))
				{
					if(buffer[0] != '#') 
						break;
				}

				// 태그를 읽고,
				tagName = strtok_s(buffer, delimiter, &tokenNext);
				tagVal = strtok_s(NULL, delimiter, &tokenNext);

				// face 20022
				// # v0Idx v1Idx v2Idx
				// 0 1 2
				if (!strcmp(tagName, "face"))
				{
					int NumFace = tagVal ? atoi(tagVal) : 0;

					for (int i = 0; i < NumFace; ++i)
					{
						// # v0Idx v1Idx v2Idx 건너 띄기
						while (fgets(buffer, 256, fp))
						{
							if(buffer[0] != '#') 
								break;
						}

						// 삼각형 정보 읽기
						int	vidx0, vidx1, vidx2;
						sscanf_s(buffer, "%d%d%d", &vidx0, &vidx1, &vidx2);

						GFace f(vidx0, vidx1, vidx2, -1, -1, -1, 0);
						pSubMesh->AddFace(f);
					}
				}

				// 피부색 재질
				GMaterial mtl = ::get_material(251 / 255.0, 206 / 255.0, 177 / 255.0);
				pSubMesh->AddMtl(mtl);
				pScene->AddMesh(pSubMesh);
				pSubMesh->InitRefBoneInfo();
			}
		}
	}

	pRootMesh->Edit(GTransf());

	return true;
}

bool saveAvatarDump(GScene *pScene, const char *fileName)
{
	return true;
}

bool loadItemDump(GScene *pScene, const char *fileName)
{
	FILE *fp;
	fopen_s(&fp, fileName, "r");
	if (!fp)
	{
		printf("Error[loadDump]: Cannot open file : %s\n", fileName);
		return false;
	}

	char currentLine[256];
	while (fgets(currentLine, 256, fp))
	{
		char spliter[] = " \t\n";
		char *tokenContext = NULL;
		char *name = strtok_s(currentLine, spliter, &tokenContext);
		char *countStr = strtok_s(NULL, spliter, &tokenContext);

		if (!strcmp(name, "#")) 
			continue;

		if (!strcmp(name, "graphicalMesh") || !strcmp(name, "physicalMesh"))
		{
			int numSubmeshes = countStr ? atoi(countStr) : 0;

			for (int submeshId = 0; submeshId < numSubmeshes; submeshId++)
			{
				GMesh *pMesh = new GMesh(name, GTransf(), FACE_TRI, NULL);
				for (int att=0; att < 2; att++)
				{
					while (fgets(currentLine, 256, fp)) if(currentLine[0] != '#') break;
					name = strtok_s(currentLine, spliter, &tokenContext);
					countStr = strtok_s(NULL, spliter, &tokenContext);

					if(!strcmp(name, "vertex"))
					{
						int numVerts = countStr ? atoi(countStr) : 0;

						for (int i=0;i < numVerts; i++)
						{
							while(fgets(currentLine, 256, fp)) if(currentLine[0] != '#') break;
							float pos[3], norm[3], uv[2];
							int boneIndex[4];
							float boneWeight[4];
							int mappedAvatarVertexIndex;
							float mappedAvatarVertexOffset[3];


							// posX posY posZ normX normY normZ texU texV boneIdx0 boneIdx1 boneIdx2 boneIdx3 boneWgt0 boneWgt1 boneWgt2 boneWgt3  mappedAvatarVertexIdx mappedAvatarVertexOffsetX mappedAvatarVertexOffsetY mappedAvatarVertexOffsetZ
							sscanf_s(currentLine, "%f %f %f %f %f %f %f %f %d %d %d %d %f %f %f %f %d %f %f %f\n",
								&pos[0], &pos[1], &pos[2],
								&norm[0], &norm[1], &norm[2],
								&uv[0], &uv[1],
								&boneIndex[0], &boneIndex[1], &boneIndex[2], &boneIndex[3],
								&boneWeight[0], &boneWeight[1], &boneWeight[2], &boneWeight[3],
								&mappedAvatarVertexIndex, &mappedAvatarVertexOffset[0], &mappedAvatarVertexOffset[1], &mappedAvatarVertexOffset[2]);

							GVertex v(GPoint3(pos[0], pos[1], pos[2]), GVector3(norm[0], norm[1], norm[2]));
							pMesh->AddVertex(v);
						}
					}
					else if(!strcmp(name, "face"))
					{
						int numFaces = countStr ? atoi(countStr) : 0;
						for(int i=0;i<numFaces;i++)
						{
							while(fgets(currentLine, 256, fp)) if(currentLine[0] != '#') break;
							int idx[3];

							// v0Idx v1Idx v2Idx
							sscanf_s(currentLine, "%d %d %d\n", &idx[0], &idx[1], &idx[2]);

							GFace f(idx[0], idx[1], idx[2]);
							pMesh->AddFace(f);
						}
					}

				}
				pScene->AddMesh(pMesh);
			}
		}
		else if(!strcmp(name, "boneHierarchy"))
		{
			int numBones = countStr ? atoi(countStr) : 0;
			for(int i=0;i<numBones;i++)
			{
				while(fgets(currentLine, 256, fp)) if(currentLine[0] != '#') break;

				int idx;
				char boneName[256] = {0, };
				char parentName[256] = {0, };

				// idx name parentName
				sscanf_s(currentLine, "%d %s %s\n", &idx, boneName, 256, parentName, 256);
			}

			for(int i=0;i<numBones;i++)
			{
			}
		}
		else if(!strcmp(name, "boneBindPose"))
		{
			int numBones = countStr ? atoi(countStr) : 0;
			for(int i=0;i<numBones;i++)
			{
				while(fgets(currentLine, 256, fp)) if(currentLine[0] != '#') break;

				//Matrix bindPose;
				float bindPose[4][4];

				// m00, m01, ..., m44		: 4x4 transform matrix, row major
				sscanf_s(currentLine, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
					&bindPose[0][0], &bindPose[0][1], &bindPose[0][2], &bindPose[0][3],
					&bindPose[1][0], &bindPose[1][1], &bindPose[1][2], &bindPose[1][3],
					&bindPose[2][0], &bindPose[2][1], &bindPose[2][2], &bindPose[2][3],
					&bindPose[3][0], &bindPose[3][1], &bindPose[3][2], &bindPose[3][3]);

				//skeleton->setBoneBindPose(i, bindPose);
			}
		}
		else if(!strcmp(name, "jointSphere"))
		{
			int numRigidBodies = countStr ? atoi(countStr) : 0;

			//Ragdoll *ragdoll = getRagdoll();
			//Skeleton *skeleton = getSkeleton();

			//if(!ragdoll && skeleton)
			//{
			//	ragdoll = new Ragdoll;

			//	for(int i=0;i<skeleton->getNumBones();i++)
			//		ragdoll->addJoint(i, skeleton->getBoneName(i));

			//	setRagdoll(ragdoll, true);
			//}

			//if(ragdoll)
			{
				for(int i=0;i<numRigidBodies;i++)
				{
					while(fgets(currentLine, 256, fp)) if(currentLine[0] != '#') break;

					int boneIndex;
					float offset[3];
					float radius;
					char name[256] = {0, };
					char connectedSphereName[256] = {0, };

					// name boneIdx offsetX offsetY offsetZ radius connectedSphereName
					sscanf_s(currentLine, "%s %d %f %f %f %f %s\n", name, 256, &boneIndex, &offset[0], &offset[1], &offset[2], &radius, connectedSphereName, 256);

					//RigidBodySphere *sphere = (RigidBodySphere*)ragdoll->getRigidBodyByJointName(ragdoll->getJointName(boneIndex));

					//if(sphere)
					//{
					//	sphere->setRadius(radius);
					//	sphere->setLocalOffset(offset);
					//}
					//else
					//{
					//	sphere = new RigidBodySphere(NULL, ragdoll->getJointName(boneIndex), name, connectedSphereName, radius, offset);
					//	ragdoll->addRigidBody(sphere, true);
					//}
				}
			}
		}
	}

	fclose(fp);

	return true;
}

bool saveItemDump(GScene *pScene, const char *fileName)
{
// 	FILE *fp;
// 	fopen_s(&fp, fileName, "w");
// 	if(!fp)
// 	{
// 		printf("Error[saveDump] : Cannot open file : %s\n", fileName);
// 		return false;
// 	}
// 
// 	for(int meshId=0;meshId<2;meshId++)
// 	{
// 		const Mesh *mesh = NULL;
// 
// 		switch(meshId)
// 		{
// 		case 0: 
// 			mesh = getMesh(); 
// 			fprintf(fp, "graphicalMesh %d\n", mesh->getNumSubMeshes()); 
// 			break;
// 		case 1: 
// 			mesh = getPhysicalMesh(); 
// 			fprintf(fp, "physicalMesh %d\n", mesh->getNumSubMeshes()); 
// 			break;
// 		}
// 
// 		if(mesh)
// 		{
// 			for(int submeshId=0;submeshId<mesh->getNumSubMeshes();submeshId++)
// 			{
// 				const SubMesh *subMesh = mesh->getSubMesh(submeshId);
// 				int numVerts = subMesh->getNumVertices();
// 				const Vertex *verts = subMesh->getVertices();
// 				const Weight *weights = subMesh->getWeights();
// 
// 				// set somehow..
// 				int numVerts = 0;
// 				float pos[3], norm[3], uv[2];
// 				int boneIndex[4];
// 				float boneWeight[4];
// 				int mappedAvatarVertexIndex;
// 				float mappedAvatarVertexOffset[3];
// 
// 				fprintf(fp, "vertex %d\n", numVerts);
// 				fprintf(fp, "# posX posY posZ normX normY normZ texU texV boneIdx0 boneIdx1 boneIdx2 boneIdx3 boneWgt0 boneWgt1 boneWgt2 boneWgt3 mappedAvatarVertexIdx mappedAvatarVertexOffsetX mappedAvatarVertexOffsetY mappedAvatarVertexOffsetZ\n");
// 
// 				for(int i=0;i<numVerts;i++)
// 				{
// 					const Vertex &v = verts[i];
// 					const Weight &w = weights[i];
// 					fprintf(fp, "%f %f %f %f %f %f %f %f %d %d %d %d %f %f %f %f %d %f %f %f\n",
// 						pos[0], pos[1], pos[2],
// 						norm[0], norm[1], norm[2],
// 						uv[0], uv[1], 
// 						boneIndex[0], boneIndex[1], boneIndex[2], boneIndex[3],
// 						boneWeight[0], boneWeight[1], boneWeight[2], boneWeight[3],
// 						mappedAvatarVertexIndex, mappedAvatarVertexOffset[0], mappedAvatarVertexOffset[1], mappedAvatarVertexOffset[2]);
// 				}
// 
// 				int numFaces = subMesh->getNumIndices() / 3;
// 				const int *idxs = subMesh->getIndices();
// 
// 				// set somehow..
// 				int numFaces;
// 				const int *idxs;
// 
// 				fprintf(fp, "face %d\n", numFaces);
// 				fprintf(fp, "# v0Idx v1Idx v2Idx\n");
// 
// 				for(int i=0;i<numFaces;i++)
// 				{
// 					fprintf(fp, "%d %d %d\n", idxs[i*3+0], idxs[i*3+1], idxs[i*3+2]);
// 				}
// 			}
// 		}
// 	}
// 
// 	Skeleton *skeleton = getSkeleton();
// 	int numBones = skeleton->getNumBones();
// 	int numBones;
// 
// 	fprintf(fp, "boneHierarchy %d\n", numBones);
// 	fprintf(fp, "# idx name parentName\n");
// 	for(int i=0;i<numBones;i++)
// 	{
// 		// set somehow..
// 		int boneIndex;
// 		char boneName[256];
// 		char parentName[256];
// 
// 		fprintf(fp, "%d %s %s\n", boneIndex, boneName, parentName);
// 	}
// 
// 	fprintf(fp, "boneBindPose %d\n", numBones);
// 	fprintf(fp, "# m00, m01, ..., m44		: 4x4 transform matrix, row major\n");
// 
// 	for(int i=0;i<numBones;i++)
// 	{
// 		Matrix bindPose = skeleton->getBoneBindPose(i);
// 
// 		// set somehow..
// 		float bindPose[4][4];
// 		fprintf(fp, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
// 			bindPose[0][0], bindPose[0][1], bindPose[0][2], bindPose[0][3],
// 			bindPose[1][0], bindPose[1][1], bindPose[1][2], bindPose[1][3],
// 			bindPose[2][0], bindPose[2][1], bindPose[2][2], bindPose[2][3],
// 			bindPose[3][0], bindPose[3][1], bindPose[3][2], bindPose[3][3]);
// 	}
// 
// 	Ragdoll *ragdoll = getRagdoll();
// 	int numRigidBodies = ragdoll->getNumRigidBodies();
// 
// 	// set somehow..
// 	int numRigidBodies;
// 
// 	fprintf(fp, "jointSphere %d\n", numRigidBodies);
// 	fprintf(fp, "# name boneIdx offsetX offsetY offsetZ radius connectedSphereName\n");
// 
// 	for(int i=0;i<numRigidBodies;i++)
// 	{
// 		RigidBodySphere *sphere = (RigidBodySphere*)ragdoll->getRigidBody(i);
// 		const float *offset = sphere->getLocallOffset();
// 
// 		// set somehow..
// 		int boneIndex;
// 		float offset[3];
// 		float radius;
// 		char name[256] = {0, };
// 		char connectedSphereName[256] = {0, };
// 
// 		fprintf(fp, "%s %d %f %f %f %f %s\n", name, boneIndex, offset[0], offset[1], offset[2], radius, connectedSphereName);
// 	}
// 
// 	fclose(fp);
	return true;
}