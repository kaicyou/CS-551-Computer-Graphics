void simpleTest()
{
	osuPerspective(90.0, 1.0, -1000.);

	float from[3]={3.0,0.0,3.0};
	float at[3] = {0.0,0.0,-8.0};
	float up[3] = {0.0, 1.0, 0.0};

	osuLookat (from, at, up);

	osuClear(0.0,0.0,0.0);


	osuDiffuse(0.0, 0.0 , 1.0 );
	osuSpecular(1.0, 1.0, 1.0, 1.0);

	float lpos[3]={0.0, 1.5, 5.0};

	osuPointLight(lpos,1.0);
	osuAmbientLight(0.4);

	//YOU MUST CONVERT THIS TO TWO TRIANGLES!!!
	osuBegin(OSU_POLYGON);
	osuVertex3f(-4.5, -1.75, -5.5);
	osuVertex3f(-4.5, 1.75, -5.5);
	osuVertex3f(4.5, 1.75, -5.5);
	osuVertex3f(4.5, -1.75, -5.5);
	osuEnd();


	osuDiffuse(1.0, 0.0 , 0.0 );
	osuSpecular(1.0, 1.0, 1.0, 1.0);

	osuColor3f(0.0,0.0,1.0);
	osuBegin(OSU_POLYGON);
	osuVertex3f(0.0, -1.75, -2.5);
	osuVertex3f(0.0, 1.75, -2.5);
	osuVertex3f(0.0, 1.75, -7.5);
	osuVertex3f(0.0, -1.75, -7.5);
	osuEnd();

}

void blueCube()
{
	osuPerspective(40, 7.5, 100);

	float from[3]={5.0,5.0,5.0};
	float at[3] = {0.0,0.0,0.0};
	float up[3] = {0.0, 1.0, 0.0};

	osuLookat (from, at, up);

	osuClear(0.0, 0.0, 0.0);
	osuClearZ();

	osuDiffuse(0.0, 0.0 , 1.0 );
	osuSpecular(1.0, 1.0, 1.0, 1.0);

	float lpos[3]={3.0, 1.5, 5.0};
	float dir[3] = {0.0, -1.0, 0.0};

	osuPointLight(lpos,0.5);
	osuDirectionalLight(dir,0.5);
	osuAmbientLight(0.4);

	//YOU MUST CONVERT THESE TO USE TRIANGLES!!!
	//back
	osuBegin(OSU_POLYGON);
	osuVertex3f( -1, -1, -1);
	osuVertex3f(  1, -1, -1);
	osuVertex3f(  1,  1, -1);
	osuVertex3f( -1,  1, -1);
	osuEnd();

	//right
	osuBegin(OSU_POLYGON);
	osuVertex3f(  1, -1, -1);
	osuVertex3f(  1 ,-1,  1);
	osuVertex3f ( 1 , 1,  1);
	osuVertex3f ( 1 , 1, -1);
	osuEnd();


	//front
	osuBegin(OSU_POLYGON);
	osuVertex3f( -1, -1,  1);
	osuVertex3f( -1,  1,  1);
	osuVertex3f(  1,  1,  1);
	osuVertex3f(  1, -1,  1);
	osuEnd();

	//top
	osuBegin(OSU_POLYGON);
	osuVertex3f( -1,  1, -1);
	osuVertex3f(  1,  1, -1);
	osuVertex3f(  1,  1,  1);
	osuVertex3f( -1,  1,  1);
	osuEnd();

	
	//bottom
	osuBegin(OSU_POLYGON);
	osuVertex3f( -1, -1, -1);
	osuVertex3f( -1, -1,  1);
	osuVertex3f(  1, -1,  1);
	osuVertex3f(  1 ,-1, -1);
	osuEnd();


	//left
	osuBegin(OSU_POLYGON);
	osuVertex3f( -1, -1, -1);
	osuVertex3f( -1,  1 ,-1);
	osuVertex3f( -1,  1,  1);
	osuVertex3f( -1, -1,  1);
	osuEnd();
}
