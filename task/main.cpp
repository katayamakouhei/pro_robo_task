#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#include <vector>

#include <GL/glut.h>

#include "include/pf.h"
#include "include/pf_amcl.h"
#include "include/draw.h"
#include "include/param.h"

Amcl amcl;
Map map;
GlDraw glDraw;


Robot robot;
ParticleSet particle_set;
Control control_t;

bool mapRead(){
    FILE *mapFile;
    int mapStatus;  //0：フリー，1：壁,2:ボール

    if((mapFile =fopen("../map_wall.txt","r")) == NULL){
        std::cout << "Doesn't exist map file" << std::endl;
        return false;
    }else{
        int i=0;
        int j=0;
        map = std::vector< std::vector<int> >(MAP_LINE, std::vector<int>(MAP_COLUMN, 0));   //mapの配列数の宣言:map[LINE][COLUMN]
        while (fscanf(mapFile, "%d", &mapStatus) != EOF){
            if(j==(MAP_COLUMN-1)){
                map[i][j] = mapStatus;
                j = 0;
                i++;
            }else{
                j++;
                map[i][j] = mapStatus;
            }
        }
        return true;
    }
    fclose(mapFile);
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    //地図の描画
    glDraw.drawMap(map);
    //動作モデル
    amcl.pfMotionUpdata(&control_t, &robot, &particle_set);
    //地図からそれぞれ更新
    amcl.pfGetSensorData(map, &robot, &particle_set);
    //計測モデル
    amcl.pfSensorUpdata(&robot, &particle_set);
    //リサンプリング
    amcl.pfResampling(&robot, &particle_set);
    //ロボット，パーティクルの描画
    glDraw.drawRobotAndParticle(&robot, &particle_set);
    control_t.trance = control_t.rotate = 0;
    glFlush();
}

void resize(int w, int h){
	glViewport(0, 0, w, h);
	glLoadIdentity();
	glOrtho(-20,600,-500,30, -1.0, 1.0);   //画面サイズの変更
}


void keyboard(unsigned char key, int x, int y){
  switch (key) {
    case 'q':
    case 'Q':
    case '\033':  // '\033' は ESC の ASCII コード
      exit(0);
	  break;

    case 'w':   //前進
        control_t.trance = CONTROL_TRANCE;
        glutPostRedisplay();
        //std::cout << "up" << std::endl;
    break;

    case 'a':   //左回転
        control_t.rotate = CONTROL_ROTATE;
        glutPostRedisplay();
        //std::cout << "left" << std::endl;
    break;

    case 's':   //後退
        control_t.trance = -CONTROL_TRANCE;
        glutPostRedisplay();
        //std::cout << "back" << std::endl;
    break;

    case 'd':   //右回転
        control_t.rotate = -CONTROL_ROTATE;
        glutPostRedisplay();
        //std::cout << "right" << std::endl;
    break;

    case 'r':   //初期位置に戻す
        amcl.pfInit(&robot, &particle_set);
        glutPostRedisplay();
        //std::cout << "Restart" << std::endl;
    break;

    default:
    break;
  }
}


int main(int argc, char **argv){
    amcl.pfInit(&robot, &particle_set);
    if(mapRead()){
        glutInit(&argc, argv);
        glutInitWindowPosition(100, 100);
        glutInitWindowSize(400, 400);
        glutCreateWindow("Amcl_task");
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glutDisplayFunc(display);
        glutReshapeFunc(resize);
        glutKeyboardFunc(keyboard);
        glutMainLoop();
        return 0;
    }else{
        return -1;
    }

}
