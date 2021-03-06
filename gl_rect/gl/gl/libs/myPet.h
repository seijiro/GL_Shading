//myPet.h
//四足の仮想動物
//rootは胴体の中心

#define MAXJOINT 20
#define MAXFRAME 5000//許容最大コマ数
#define MAXKEYFRAME 10//最大キーフレーム数

class CmyPet
{
public:
	CmyPet(void);
	~CmyPet(void) { };
	//メンバ変数
	int numJoint;//関節個数

	float* diffuse ;//拡散光反射率
	float* specular;//鏡面光反射率
	float* ambient; //環境光反射率
	float* shadowDiffuse; //影の拡散光
	float* shadowSpecular; //影の鏡面光
	float highlight;//光沢

	bool flagShadow;

	//位置(Rootは胴体の中心)
  CVector vPos;
  //姿勢角
  CVector vAng;
  //進行方向
  float dir;
	//関節角の配列
	CVector joint[MAXJOINT];
	//胴体
	CVector trunk;
	CVector trunkBefore;
	CVector trunkAfter;
  //首
	CVector neck;
	//頭部
	CVector head ;
  //目
  CVector eye;
	//鼻
	CVector nose;
  //耳
  CVector ear;
	//口
	CVector mouth;
	//脚（前脚，後脚)
	CVector leg1, leg2, foot;
  //尾
  CVector tail;
  //脚を取り付ける位置
	CVector legPos;//胴体中心からの相対位置
  //脚の全体の長さ
  float legLen;
	//animation用
	CVector f_vPos[MAXFRAME];
	CVector f_vAng[MAXFRAME];
  float  f_dir[MAXFRAME];
	CVector k_vPos[MAXKEYFRAME];
	CVector k_vAng[MAXKEYFRAME];
  float  k_dir[MAXKEYFRAME];
	CVector f_joint[MAXFRAME][MAXJOINT];
	CVector k_joint[MAXKEYFRAME][MAXJOINT];
	float actTime[MAXKEYFRAME];//keyFrame間の動作時間
  float unitTime;//1フレーム当たりの描画時間
	int numKeyFrame;//ｷｰﾌﾚｰﾑ数
	int numFrame0;//各オブジェクトの1つのシーンの全表示ﾌﾚｰﾑ数

	//メンバ関数
	void draw();
	void draw(int n);
  void reset();
	void walk(float dist, float pace, float tt);
  void turn(float angle, float tt);
	void stand(float tt);
	void initPose(float tt);
	void wait(float tt);
	void swingTail(int n, float tt);
	void upTail(float tt);
	void downTail(float tt);
private:
  void drawTrunk();
  void drawHead();
  void drawLeg();
  void drawLeg1(int jNo);
  void drawLeg2(int jNo);
  void drawTail();
	void initKeyAnimation();
	void makeFrameData();
  void walkRight(float pace, float tt);
  void walkLeft(float pace, float tt);
};
//----------------------------------------------------------------
CmyPet::CmyPet()
{
	int i;

	static float diffuse0[] = {0.7f, 0.6f, 0.4f, 1.0f};
  static float ambient0[] = {0.5f, 0.4f, 0.3f, 1.0f};
	static float specular0[] = {0.8f, 0.8f, 0.8f, 1.0f};
	static float shadowD[] = {0.2f, 0.2f, 0.2f, 0.3f};
	static float shadowS[] = {0.0f, 0.0f, 0.0f, 1.0f};

	//仮想動物のマテリアル特性
	diffuse = diffuse0;
  ambient = ambient0;
  specular = specular0;
  highlight = 100.0f;
	shadowDiffuse = shadowD;
	shadowSpecular = shadowS;
	flagShadow = false;

	unitTime = 0.01;//1フレーム当たりの描画時間（仮の値)
	numJoint = 11;
  vPos = CVector();
  vAng = CVector();
	for(i = 0; i< numJoint; i++) joint[i] = CVector();

  //胴体
	trunk = CVector( 0.2, 0.18, 0.4);
  trunkBefore = CVector(0.22, 0.22, 0.22);
	trunkAfter = CVector(0.22, 0.22, 0.22);
	//頭部
	head = CVector( 0.23, 0.2, 0.23);
  //目
//  eye = CVector( 0.04, 0.04, 0.04);
  eye = CVector( 0.06, 0.06, 0.06);
	//鼻
	nose = CVector( 0.03, 0.03, 0.03);
  //耳
  ear = CVector( 0.06, 0.15, 0.06);
	//口
	mouth = CVector(0.14, 0.14, 0.14);
	//上脚（前脚，後脚)
	leg1 = CVector(0.07, 0.06, 0.07);
	//下脚（前脚，後脚)
	leg2 = CVector(0.065, 0.06, 0.065);
  //尾
  tail = CVector( 0.1, 0.1, 0.13);
  //脚を取り付ける位置
	legPos.x = trunk.x / 2.0 - 0.03;
  legPos.y = trunk.y / 2.0 - 0.03;
  legPos.z = trunk.z / 2.0 - 0.04;
  //脚全長
  legLen = leg1.y + leg2.y;
	//Rootの位置（胴体の中心)　
	vPos.y = legLen + legPos.y ;
}
//--------------------------------------------------------------------------------------
void CmyPet:: draw(int n)
{
  int j;
  //n番目のフレーム番号の姿勢を描画
  if(n > numFrame0) n = numFrame0;

  vPos = f_vPos[n];
	vAng = f_vAng[n];
	dir = f_dir[n];
  for(j = 0; j < numJoint; j++) joint[j] = f_joint[n][j];
  draw();
}


//------------------------------------------------------------------
void CmyPet::draw()
{
	if(flagShadow == true)
	{
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, shadowDiffuse);
		glMaterialfv(GL_FRONT,GL_SPECULAR,shadowSpecular);
	}
	else
	{
		glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
		glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
		glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
		glMaterialf(GL_FRONT,GL_SHININESS,highlight);
	}

  //現在位置(全体を胴体の中心まで持ち上げる）
  glTranslatef(vPos.x, vPos.y, vPos.z);
  //全体の姿勢
  glRotatef(vAng.y, 0.0, 1.0, 0.0); //y軸回転
  glRotatef(vAng.x, 1.0, 0.0, 0.0); //x軸回転
  glRotatef(vAng.z, 0.0, 0.0, 1.0); //z軸回転
  drawTrunk();//胴体
  drawHead();//首も含めた頭部
  drawLeg();
  drawTail();
}
//------------------------------------------------------------------------------
void CmyPet::drawTrunk(void)
{
	//胴体
	glPushMatrix();
		glScalef(trunkBefore.x, trunkBefore.y, trunkBefore.z);
		glTranslatef(0.0, 0.0, trunk.z);
		drawSphere(0.5, 10, 10);
	glPopMatrix();
	glPushMatrix();
		glScalef(trunkAfter.x, trunkAfter.y, trunkAfter.z);
		glTranslatef(0.0, 0.0, -trunk.z);
		drawSphere(0.5, 10, 10);
	glPopMatrix();


}
//-----------------------------------------------------------------------------
void CmyPet::drawHead()
{
	static float diffuseEye[] = {0.1f, 0.1f, 0.2f, 1.0f};
  glPushMatrix();

  //首の位置で平行移動
	glTranslatef( 0.0, trunk.y / 2.0 - 0.02, trunk.z / 2.4 );
  //首回転
  glRotatef(joint[0].y, 0.0, 1.0, 0.0);//y軸回転
  glRotatef(-90.0 + joint[0].x, 1.0, 0.0, 0.0);//x軸回転
  glRotatef(joint[0].z, 0.0, 0.0, 1.0);//z軸回転

  glTranslatef( 0.0, 0.0, neck.z / 2.0 + head.z / 2.0);//首の先端まで平行移動
  //頭
	glPushMatrix();
		glScalef(head.x, head.y, head.z);
		drawSuper(0.5, 8, 8, 0.9, 0.8);
	glPopMatrix();
	
	//目の色
	if(flagShadow) {
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, shadowDiffuse);
		glMaterialfv(GL_FRONT,GL_SPECULAR,shadowSpecular);
	}
	else{
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, diffuseEye);		
		glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
	}
  //右目
  glPushMatrix();
    glTranslatef( -head.x / 4.0, - head.y / 2.7, head.z / 3.0 );//
    glScalef(eye.x, eye.y, eye.z);
    drawSphere(0.5, 8, 8);
  glPopMatrix();
  //左目
  glPushMatrix();
    glTranslatef( head.x / 4.0, - head.y / 2.7, head.z / 3.0 );//
    glScalef(eye.x, eye.y, eye.z);
    drawSphere(0.5, 8, 8);
  glPopMatrix();
  //鼻
  glPushMatrix();
    glTranslatef( 0.0, - head.y / 1.2, head.z / 6.0 );//
    glScalef(nose.x, nose.y, nose.z);
    drawSphere(0.5, 8, 8);
  glPopMatrix();

	if(flagShadow){ 
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, shadowDiffuse);
		glMaterialfv(GL_FRONT,GL_SPECULAR,shadowSpecular);
	}
	else
	{
		glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
		glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
		glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
	}
  //右耳
  glPushMatrix();
    glTranslatef( -head.x / 2.2, 0.0, head.z / 4.0);//
    glRotatef(90.0 + joint[10].x, 1.0, 0.0, 0.0);//x軸回転
    glScalef(ear.x, ear.y, ear.z);
    glTranslatef(0.0, -0.5, 0.0);
    drawSphere(0.5, 10, 10);
  glPopMatrix();
  //左耳
  glPushMatrix();
    glTranslatef( head.x / 2.2, 0.0, head.z / 4.0);//
    glRotatef(90.0 + joint[10].x, 1.0, 0.0, 0.0);//x軸回転
    glScalef(ear.x, ear.y, ear.z);
    glTranslatef(0.0, -0.5, 0.0);
    drawSphere(0.5, 10, 10);
  glPopMatrix();
	//口
	glPushMatrix();
    glTranslatef( 0.0, -head.y / 2.0, 0.0);//
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glScalef(mouth.x, mouth.y, mouth.z);
		drawSuper(0.5, 8, 8, 0.5, 0.7);
	glPopMatrix();

  glPopMatrix();
}
//------------------------------------------------------------------
void CmyPet::drawLeg()
{
  //前右脚
  glPushMatrix();
    // 前右脚の位置へ平行移動
    glTranslatef(-legPos.x, -legPos.y, legPos.z);
    drawLeg1(1);
    drawLeg2(5);
	glPopMatrix();
  //前左脚
  glPushMatrix();
    // 前左脚の位置へ平行移動
    glTranslatef( legPos.x, -legPos.y, legPos.z);
    drawLeg1(2);
    drawLeg2(6);
  glPopMatrix();
  //後右脚
  glPushMatrix();
    // 後右脚の位置へ平行移動
    glTranslatef(-legPos.x, -legPos.y, -legPos.z);
    drawLeg1(3);
    drawLeg2(7);
	glPopMatrix();
  //後左脚
  glPushMatrix();
    // 後左脚の位置へ平行移動
    glTranslatef( legPos.x, -legPos.y, -legPos.z);
    drawLeg1(4);
    drawLeg2(8);
	glPopMatrix();
}
//----------------------------------------------------------------------------
void CmyPet::drawLeg1(int jNo)
{
  //脚付け根の回転
  glRotatef(joint[jNo].y, 0.0, 1.0, 0.0);//y軸回転
  glRotatef(joint[jNo].x, 1.0, 0.0, 0.0);//x軸回転
  glRotatef(joint[jNo].z, 0.0, 0.0, 1.0);//z軸回転
  glPushMatrix();
    glScalef(leg1.x, leg1.y + 0.01, leg1.z );
    glTranslatef(0.0, -0.5, 0.0);//原点の位置へ下げる
		drawSuper(0.5, 8, 8, 0.5, 0.8);
  glPopMatrix();
}
//----------------------------------------------------------------------------
void CmyPet::drawLeg2(int jNo)
{
  //下脚の位置へ平行移動
  glTranslatef(0.0, -leg1.y, 0.0);
  //膝の回転
  glRotatef(joint[jNo].y, 0.0, 1.0, 0.0);//y軸回転
  glRotatef(joint[jNo].x, 1.0, 0.0, 0.0);//x軸回転
  glRotatef(joint[jNo].z, 0.0, 0.0, 1.0);//z軸回転
  glPushMatrix();
    glScalef(leg2.x, leg2.y, leg2.z);
    glTranslatef(0.0, -0.5, 0.0);//原点の位置へ下げる
	  drawSuper(0.5, 8, 8, 0.5, 0.8);
  glPopMatrix();
}
//----------------------------------------------------------------------------
void CmyPet::drawTail()
{
  glPushMatrix();
    //尾の位置へ平行移動
    glTranslatef( 0.0, 0.0, -trunk.z / 2.2 );
    //尾の回転
    glRotatef(joint[9].y, 0.0, 1.0, 0.0);//y軸回転
    glRotatef(30.0 + joint[9].x, 1.0, 0.0, 0.0);//x軸回転
    glRotatef(joint[9].z, 0.0, 0.0, 1.0);//z軸回転
    glPushMatrix();
			glScalef(tail.x, tail.y, tail.z);
			glTranslatef(0.0, 0.0, -0.5);
      glRotatef(90.0, 1.0, 0.0, 0.0);//90度x軸回転
			drawSphere(0.5, 10, 10);
    glPopMatrix();
	glPopMatrix();
}
//----------------------------------------------------------------------------
//keyframeを作成するメンバ関数において最初にcallする
void CmyPet::initKeyAnimation()
{
	short k, j;
	//現在の位置姿勢（最初の姿勢で初期化)
	for(k = 0; k < numKeyFrame; k++)
	{
		k_vPos[k] = vPos;
    k_vAng[k] = vAng;
    k_dir[k] = dir;

		for(j = 0; j < numJoint; j++) k_joint[k][j] = joint[j];
	}
}
//----------------------------------------------------------------------------
//ﾌﾚｰﾑﾃﾞｰﾀ作成
//keyframeを作成するメンバ関数で最後にcall
void CmyPet::makeFrameData()
{
	short i, j, k, numHokan;
	float ss;

  if(unitTime == 0.0)
  {	printf("unitTime = 0となりました \n");
      return;
  }

	for(k = 1; k < numKeyFrame; k++)
  {
		if(actTime[k] < 0)
		{	printf("actTime<0となりました \n");
			return;//どこかのルーチンで負の値に設定した！！
		}

		numHokan = (short)(actTime[k] / unitTime);//補間点数
		if(numHokan == 0) continue;
		if(numFrame0 + numHokan > MAXFRAME) 
		{
			printf("フレーム数が制限を越えるおそれがあります \n");
			return;
		}
		for(i = 0; i <= numHokan; i++)
		{
			ss = (float)i / (float)numHokan;
            f_vPos[numFrame0 + i] = k_vPos[k-1] + (k_vPos[k] - k_vPos[k-1]) * ss;
            f_vAng[numFrame0 + i] = k_vAng[k-1] + (k_vAng[k] - k_vAng[k-1]) * ss;
           f_dir[numFrame0 + i] = k_dir[k-1] + (k_dir[k] - k_dir[k-1]) * ss;

			for(j = 0; j < numJoint; j++)
			{//補間データを作成(線形補間)
                f_joint[numFrame0 + i][j] = k_joint[k-1][j]
				                 + (k_joint[k][j] - k_joint[k- 1][j]) * ss;
			}
		}
		numFrame0 += numHokan;
	}
	reset();
}
//---------------------------------------------------------------------------------
//残りﾌﾚｰﾑのﾃﾞｰﾀを最終値に再設定
//makeFrameData()の最後にcallする
void CmyPet::reset()
{
	short i, j;

	//コマ数最大値の更新
	//最終値を現在値に
  if(numFrame0 > 0)
	{
    vPos = f_vPos[numFrame0];
    vAng = f_vAng[numFrame0];
    dir = f_dir[numFrame0];

		for(j = 0; j < numJoint; j++) joint[j] = f_joint[numFrame0][j];
	}

	//残りをすべて現在値
	for(i = numFrame0; i < MAXFRAME; i++)
	{
    f_vPos[i] = vPos;
    f_vAng[i] = vAng;
    f_dir[i] = dir;

		for(j = 0; j < numJoint; j++) f_joint[i][j] = joint[j];
	}
}
//----------------------------------------------------------------
//与えられた距離歩く
void CmyPet::walk(float dist,  float pace, float time)//
{                    //距離, 歩幅 , 一歩当たりの所要時間,微小時間増分     　　　　　　
	short i, numStep, numStep2;
	float rest;

	dir = vAng.y;//現在向いている方向へ
	numStep = (short)(dist / pace);
	//右足,左足2歩を単位とした歩数
	numStep2 = numStep / 2;
	if(dist < 2.0 * pace) rest = dist;
	else rest = dist - 2.0 * pace * (float)numStep2;

	for(i = 0;i < numStep2; i++)
	{
		walkRight(pace, time);//前右，後左
		walkLeft(pace, time); //前左，後右
	}
	//残り(1歩以内なら右足で)
	if(rest < pace) walkRight(rest, time * rest / pace);
	else	
	{	
		walkRight(pace, time);
    rest -= pace;
		if(rest > 0.0) walkLeft(rest, time * rest / pace);
	}
}
//---------------------------------------------------------------------
void CmyPet::walkRight(float pace, float time)
{
  float pace2 = pace / 2.0;//半歩
  float pp = M_PI / 180.0;
  //股関節角度
	float paceA = 180.0 * asin(pace2 / legLen) / M_PI;
	float cc = pace2 * cos(pp * dir) ;//半歩のz成分(基本姿勢ではz方向が前）
	float ss = pace2 * sin(pp * dir) ;//x成分

	numKeyFrame = 3;
 	initKeyAnimation();
	//action1(右前脚一歩前へ)
	k_joint[1][1].x = -paceA;//前右
	k_joint[1][2].x = paceA; //前左
	k_joint[1][3].x = paceA;//後右
	k_joint[1][4].x = -paceA; //後左
	k_vPos[1].z = k_vPos[0].z + cc;
	k_vPos[1].x = k_vPos[0].x + ss;
  k_vPos[1].y = legLen * cos(pp * k_joint[1][1].x) + legPos.y;//y座標
	actTime[1] = time / 2.0;

	//action2(基本姿勢に戻る)
	k_joint[2][1].x = 0.0;//前右
	k_joint[2][2].x = 0.0; //前左
	k_joint[2][3].x = 0.0;//後右
	k_joint[2][4].x = 0.0; //後左
	k_vPos[2].z = k_vPos[1].z + cc;
	k_vPos[2].x = k_vPos[1].x + ss;
  k_vPos[2].y = legLen * cos(pp * k_joint[2][1].x) + legPos.y;//z座標
	actTime[2] = time / 2.0;

	makeFrameData();
}
//---------------------------------------------------------------------
void CmyPet::walkLeft(float pace, float time)
{
  float pace2 = pace / 2.0;
  float pp = M_PI / 180.0;
  //股関節角度
	float paceA = 180.0 * asin(pace2 / legLen) / M_PI;
	float cc = pace2 * cos(pp * dir);//半歩のz成分
	float ss = pace2 * sin(pp * dir);//x成分

	numKeyFrame = 3;
 	initKeyAnimation();
	//action1(右前脚一歩前へ)
	k_joint[1][1].x = paceA;//前右
	k_joint[1][2].x = -paceA; //前左
	k_joint[1][3].x = -paceA;//後右
	k_joint[1][4].x = paceA; //後左
	k_vPos[1].z = k_vPos[0].z + cc;
	k_vPos[1].x = k_vPos[0].x + ss;
  k_vPos[1].y = legLen * cos(pp * k_joint[1][1].x) + legPos.y;//z座標
	actTime[1] = time / 2.0;

	//action2(基本姿勢に戻る)
	k_joint[2][1].x = 0.0;//前右
	k_joint[2][2].x = 0.0; //前左
	k_joint[2][3].x = 0.0;//後右
	k_joint[2][4].x = 0.0; //後左
	k_vPos[2].z = k_vPos[1].z + cc;
	k_vPos[2].x = k_vPos[1].x + ss;
  k_vPos[2].y = legLen * cos(pp * k_joint[2][1].x) + legPos.y;//z座標
	actTime[2] = time / 2.0;

	makeFrameData();
}
//--------------------------------------------------------------------------
//正面方向の回転(angle>0で上から見て左回転)
void CmyPet::turn(float angle, float time)
{
	numKeyFrame = 2;
	initKeyAnimation();

	//現在の各関節の角度
	k_vAng[1].y = vAng.y + angle;
	actTime[1] = time;
	makeFrameData();
	dir = vAng.y;
}

//後ろ脚で立ち上がる
void CmyPet::stand(float time)
{
	numKeyFrame = 2;
	initKeyAnimation();

	k_joint[1][3].x = 90.0;//後右
	k_joint[1][4].x = 90.0; //後左
  k_vPos[1].y = legLen*0.8 + trunk.z / 2.0;//y座標
	k_vAng[1].x = -90.0;
	k_joint[1][0].x = 90.0;
	actTime[1] = time ;
	makeFrameData();
}

//基本姿勢に戻る
void CmyPet::initPose(float time)
{
	short j;
	
	numKeyFrame = 2;
	initKeyAnimation();

	for(j = 0; j < numJoint; j++) k_joint[1][j] = CVector();
	k_vAng[1].x = 0.0;
	k_vAng[1].z = 0.0;
	k_vPos[1].y = legLen + legPos.y;//rootの位置
	actTime[1] = time;
	makeFrameData();
}

void CmyPet::wait(float time)
{
	numKeyFrame = 2;
	initKeyAnimation();
	actTime[1] = time;
	makeFrameData();
}

void CmyPet::swingTail(int n, float time)
{
	short i;
	
	for(i = 0; i < n; i++) 
	{
		upTail(time/2.0);
		downTail(time/2.0);
	}
}

void CmyPet::upTail(float time)
{
	numKeyFrame = 2;
	initKeyAnimation();
	k_joint[1][9].x += 60.0;
	actTime[1] = time;
	makeFrameData();
}

void CmyPet::downTail(float time)
{
	numKeyFrame = 2;
	initKeyAnimation();
	k_joint[1][9].x -= 60.0;
	actTime[1] = time;
	makeFrameData();
}

 