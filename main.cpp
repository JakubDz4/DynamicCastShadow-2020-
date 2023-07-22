#include <SDL2/SDL.h>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <fstream>

using namespace std;

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int RayRadius = sqrt((SCREEN_HEIGHT/2)*(SCREEN_HEIGHT/2)+(SCREEN_WIDTH/2)*(SCREEN_WIDTH/2))*2;
static const double TWOPI = 6.2831853071795865;
static const double RAD2DEG = 57.2957795130823209;
const double Deegre = 0.001;
double angle;
int mx,my;
int ConstInterPointsNumb;
int RayVal;
bool opcja = true;

struct doublePoint
{
    double x;
    double y;
};

struct box
{
    vector <int> vNumbOfNope;
    vector <doublePoint> vNumbOfPoints;
    bool succes = true;
};

doublePoint MainLine[2] = {SCREEN_WIDTH/2,SCREEN_HEIGHT/2,NULL,NULL};
doublePoint InterPoint;

box loadMap( string path )
{
    ifstream plik;
    box dane;
    plik.open(path.c_str(),ios::in);
    if(plik.good()==false)
        {
            dane.succes = false;
            return dane;
        }

    string read;
    int i=-1;
    vector <int> vectNumbOfNope;
    vector <doublePoint> vectNumbOfPoints;

    while(getline(plik,read))
    {
        if(read[0]=='#')
        {

            vectNumbOfNope.push_back(i);
        }
        else
        {
        doublePoint tmp;
        float x,y;
        sscanf(read.c_str(),"%f %f",&x,&y);
        tmp.x = x; tmp.y = y;
        vectNumbOfPoints.push_back(tmp);//(atof(read.c_str()));
        i++;
        }
    }
    plik.close();
    dane.vNumbOfNope = vectNumbOfNope;
    dane.vNumbOfPoints = vectNumbOfPoints;

    vectNumbOfNope.clear();
    vectNumbOfPoints.clear();

    return dane;
}

double CalcAngle(doublePoint P)
{
    return atan2(P.x-MainLine[0].x,P.y-MainLine[0].y);
}

void IntersectionPointsSort(doublePoint InterPoints[])
{
    for(int i=1; i<RayVal; i++)
    {
        for(int j=RayVal-1; j>=1; j--)
        {
            if(CalcAngle(InterPoints[j])<CalcAngle(InterPoints[j-1]))
            {
                swap(InterPoints[j],InterPoints[j-1]);
            }
        }
    }
}

void SightAlgorithm (doublePoint InterPoints[])
{
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_TRIANGLES);
    for(int i=0;i<RayVal-1;i++)
    {
        glVertex2f( InterPoints[i].x, InterPoints[i].y );
        glVertex2f( MainLine[0].x, MainLine[0].y );
        glVertex2f( InterPoints[i+1].x, InterPoints[i+1].y );
    }
    glVertex2f( InterPoints[0].x, InterPoints[0].y );
    glVertex2f( MainLine[0].x, MainLine[0].y );
    glVertex2f( InterPoints[RayVal-1].x, InterPoints[RayVal-1].y );
    glEnd();
}

void init()
{
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -10, 10.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
}

void lines(doublePoint r, doublePoint g)
{
    glColor3f(1.0,1.0,0.0);
    glBegin(GL_LINES);
        glVertex2f( r.x, r.y );
        glVertex2f( g.x, g.y );
    glEnd();

}

void DrawPlayer(int x, int PlayerX, int PlayerY)
{
    double z;
    int X=PlayerX-x,Y=PlayerY-x;
    for(int i=0; i<=4*x*x+4*x; i++)///4*x*x+4*x poniewaz kula jest "szerokosci" tak naprawde (jezeli x = 4) 9x9, dlatego bo liczymy 0
    {
            z = round(sqrt((PlayerX-X)*(PlayerX-X)+(PlayerY-Y)*(PlayerY-Y)));
            if(z<=x)
            {
                glBegin(GL_POINTS);
                    glVertex2f( X, Y );
                glEnd();
            }
        X++;
        if(X>PlayerX+x)
        {
            X=PlayerX-x;
            Y++;
        }
    }
}

void restartIntersection ()
{
    InterPoint.x = NULL;
    InterPoint.y = NULL;
}

void CalculateClosestIntPoint(double x,double y,double x2,double y2,double Px,double Py) ///x dopiero co oblicznony punkt, x2 - stary punkt
{
    if(InterPoint.x==NULL||((x-Px)*(x-Px)+(y-Py)*(y-Py)<(x2-Px)*(x2-Px)+(y2-Py)*(y2-Py)))
    {
        InterPoint={x,y};
    }
}

bool InterSection(doublePoint r, doublePoint g, doublePoint r2, doublePoint g2)///Mainline0 mainline1 point0 point1
{
    if(!opcja)
    lines(r,g);
    lines(r2,g2);
    double A1,A2,B1,B2,C1,C2;
    A1 = g.y - r.y;
    A2 = g2.y - r2.y;
    B1 = r.x - g.x;
    B2 = r2.x - g2.x;
    C1 = A1*r.x + B1*r.y;
    C2 = A2*r2.x + B2*r2.y;

    if(A1/B1==A2/B2)
        {
            if(C1==C2)
            return true;///linie sie pokrywaja
            return false;///linie sa rownolegle ale sie nie pokrywaja
        }

   // double denominator = (A1*B2 - A2*B1);
    double cx,cy;
    cx = (B2*C1 - B1*C2)/(A1*B2 - A2*B1);
    cy = (A1*C2 - A2*C1)/(A1*B2 - A2*B1);

    if( cx>=min(r2.x, g2.x) && cx<=max(r2.x, g2.x) && cy>=min(r2.y, g2.y) && cy<=max(r2.y, g2.y) ///dla lin
       && cx>=min(r.x, g.x) && cx<=max(r.x, g.x) && cy>=min(r.y, g.y) && cy<=max(r.y, g.y)) ///dla lini glownej (czy styka sie)
    {
        CalculateClosestIntPoint(cx, cy, InterPoint.x, InterPoint.y, r.x, r.y); ///cx, cy - potencalny punkt intersekcji
        return true;
    }
    else return false;
}

int main( int argc, char*args[] )
{
    box val = loadMap("plik");
    if(!val.succes)
        return -1;
     ///ladowanie
    ConstInterPointsNumb = val.vNumbOfPoints.size()+5;
    int nope[val.vNumbOfNope.size()];

    for(int i=0;i<val.vNumbOfNope.size();i++)
    {
        nope[i]=val.vNumbOfNope[i];
    }
    val.vNumbOfNope.clear();

     doublePoint point[ConstInterPointsNumb];
    for(int i=0;i<val.vNumbOfPoints.size();i++)
    {
        point[i]=val.vNumbOfPoints[i];
    }

    point[ConstInterPointsNumb-5] =  {0,0};
    point[ConstInterPointsNumb-4] =  {SCREEN_WIDTH,0};
    point[ConstInterPointsNumb-3] =  {SCREEN_WIDTH,SCREEN_HEIGHT};
    point[ConstInterPointsNumb-2] =    {0,SCREEN_HEIGHT};
    point[ConstInterPointsNumb-1] =    {0,0};
    val.vNumbOfPoints.clear();

    RayVal = ConstInterPointsNumb*3;
    doublePoint InterPoints[RayVal];
     ///

    SDL_Window* window = NULL;

    SDL_Surface* ScreenSurface = NULL;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("could not initialize SDL_INIT_VIDEO %s\n", SDL_GetError());
    }
    else
    {
        window = SDL_CreateWindow("DynamiczneSwiatla",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_OPENGL);
        SDL_GLContext Context = SDL_GL_CreateContext(window);
        init();
        if(window == NULL)
        {
            printf("could not create window %s\n", SDL_GetError());
        }
        else
        {
            bool running = true;
            bool mousebutton = false;
            SDL_Event e;
            while(running)
            {
                while(SDL_PollEvent(&e) != 0)
                {
                    if( e.type == SDL_QUIT )
					{
						running = false;
					}
					else if(e.type == SDL_KEYDOWN)
                    {
                        switch(e.key.keysym.sym)
                        {
                            case SDLK_1:
                            {if(opcja) opcja = false; else opcja = true;
                            }
                            break;
                            case SDLK_ESCAPE:
                                running = false;
                        }
                    }
					else if(e.type == SDL_MOUSEBUTTONDOWN)
                    {
                        if(mousebutton)
                            mousebutton = false;
                        else mousebutton = true;
                    }
					else if(e.type == SDL_MOUSEMOTION && mousebutton)
                    {
                        SDL_GetMouseState(&mx,&my);
                        MainLine[0].x = mx;
                        MainLine[0].y = my;
                        ///aby nie znikalo
                        if(MainLine[0].x==0)
                            MainLine[0].x++;
                        if(MainLine[0].y==0)
                            MainLine[0].y++;
                    }

                }
                glClear(GL_COLOR_BUFFER_BIT);
                for(int j=0;j<RayVal;j+=3)
                {
                    for(int k=-1;k<2;k++)
                   {
                        if(k!=0)///jezeli to "srodkowy punkt to nie obliczaj
                        {angle=atan2(MainLine[0].x-point[j/3].x,MainLine[0].y-point[j/3].y);
                        if (angle < 0.0)
                        angle += TWOPI;
                        angle = -RAD2DEG * angle;}

                        MainLine[1].x = point[j/3].x + Deegre*k*cos(angle*M_PI/180);///cos
                        MainLine[1].y = point[j/3].y + Deegre*k*sin(angle*M_PI/180);///sin
                        if(k!=0)///jezeli to "srodkowy punkt to nie przesuwaj
                        {MainLine[1].x += (MainLine[1].x-MainLine[0].x)*RayRadius;
                        MainLine[1].y += (MainLine[1].y-MainLine[0].y)*RayRadius;}

                        for(int i=0;i<ConstInterPointsNumb-1;i++)///oblicza wszystkie punkty intersekcji dla jednej z glownych linii
                        {
                            for(int b=0;b<sizeof(nope)/sizeof(nope[0]);b++)
                            {if(i==nope[b])
                                i++;}
                            if(InterSection(MainLine[0], MainLine[1], point[i], point[i+1]))
                            {
                            }
                        }
                        InterPoints[j+1+k]=InterPoint;
                        restartIntersection();
                   }
                }
                if(!opcja)
                {
                    for(int g=0;g<RayVal;g++)
                    {
                        DrawPlayer(5, InterPoints[g].x, InterPoints[g].y);
                    }
                }
                if(opcja)
                {
                IntersectionPointsSort( InterPoints );
                SightAlgorithm(InterPoints);
                }
                SDL_GL_SwapWindow(window);
                SDL_Delay(16.6);
            }
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
