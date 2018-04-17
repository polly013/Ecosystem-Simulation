#include<algorithm>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<functional>
#include<iostream>
#include<iterator>
#include<random>
#include<vector>
#include<ctime>
using namespace std;
/////////////////////////////
random_device rd;
mt19937 g(rd());
mt19937 mt_rand(time(0));
mt19937::result_type seed = time(0);

std::mt19937 gen(random_device{}());
////////////////////////////

const int MAXN = 128, N = 64, M = 64;
const int Steps = 100000;

int dx[4] = {1, -1, 0, 0};
int dy[4] = {0, 0, 1, -1};

int avgBest = 0, bestBest = 0;

struct structOrganisms{
    int Grid, DaysFeed, DaysLife;//, DontMove;
}Organism[2][MAXN][MAXN];


void print (int p){
    for (int i=0; i<N; i++){
        for (int j=0; j<M; j++){
            if (Organism[p][i][j].Grid == 0) cout << " - ";
            else cout <<" " <<  Organism[p][i][j].Grid << " " ;
        }
        cout << endl;
    }

    cout << "........................................................\n";
}

void init (int NumPlankton, int NumFish, int NumShark, int PlanktonBreed, int FishBreed, int SharkBreed, int FishStarve, int SharkStarve){
    int p = 0;
    memset (Organism[p], 0, sizeof(Organism[p]));

    vector<pair<int, int> > coord;
    for (int i=0; i<N; i++)
        for (int j=0; j<M; j++)
            coord.push_back (make_pair(i,j));

    shuffle(coord.begin(), coord.end(), g);

    uniform_int_distribution<> plife(0, PlanktonBreed-1);
    uniform_int_distribution<> flife(0, FishBreed-1);
    uniform_int_distribution<> slife(0, SharkBreed-1);

    uniform_int_distribution<> ffeed(0, FishStarve-1);
    uniform_int_distribution<> sfeed(0, SharkStarve-1);

    int i = 0;
    for (; i<NumPlankton; i++){
        Organism[p][coord[i].first][coord[i].second].Grid = 1;
        Organism[p][coord[i].first][coord[i].second].DaysLife = plife(gen);
        Organism[p][coord[i].first][coord[i].second].DaysFeed = 0;
    }

    for (; i<NumPlankton+NumFish; i++){
        Organism[p][coord[i].first][coord[i].second].Grid = 2;
        Organism[p][coord[i].first][coord[i].second].DaysLife = flife(gen);
        Organism[p][coord[i].first][coord[i].second].DaysFeed = ffeed(gen);
    }

    for (; i<NumPlankton+NumFish+NumShark; i++){
        Organism[p][coord[i].first][coord[i].second].Grid = 3;
        Organism[p][coord[i].first][coord[i].second].DaysLife = slife(gen);
        Organism[p][coord[i].first][coord[i].second].DaysFeed = sfeed(gen);
    }
}

bool eat (int a, int b, int p, int pred, int Breed){
    vector<pair<int,int> > Food;

    for (int i=0; i<4; i++)
        if (Organism[p][(a + dx[i] + N) % N][(b + dy[i] + M) % M].Grid == pred-1)
            Food.emplace_back ( (a+dx[i]+N)%N, (b+dy[i]+M)%M );

    if (Food.size () == 0)
        return 0;

    std::uniform_int_distribution<> dist(0, Food.size()-1);

    int ch = dist(gen);
    int nx = Food[ch].first, ny = Food[ch].second;

    Organism[p][nx][ny].Grid = pred;
    Organism[p][nx][ny].DaysLife = Organism[p^1][a][b].DaysLife+1;
    Organism[p][nx][ny].DaysFeed = 0;

    if (Organism[p][nx][ny].DaysLife == Breed) { ///Reproduce
        Organism[p][a][b].Grid = pred;
        Organism[p][a][b].DaysFeed = Organism[p][a][b].DaysLife = 0;

        Organism[p][nx][ny].DaysLife = 0;
    }
    else Organism[p][a][b].Grid = 0;

    return 1;
}

void move (int x, int y, int p, int Starve, int Breed){ // местим организма, стоящ в координати (i,j)
    if (Organism[p^1][x][y].Grid != 1)
        if (eat (x, y, p, Organism[p^1][x][y].Grid, Breed)) return;

    if (Organism[p^1][x][y].DaysFeed+1 == Starve) {
        Organism[p][x][y].Grid = 0; Organism[p][x][y].DaysFeed = 0; Organism[p][x][y].DaysLife = 0;
        return; ///Just died :C.
    }

    vector<pair<int, int> > PossDest;

    for (int i=0; i<4; i++)
        /// можем да се преместим само, ако това поле в момента е нула, и на предишния ход не е седяло нещо с по-голям номер
        /// (защото се мести след текущото)

        if (Organism[p^1][(x + dx[i] + N) % N][(y + dy[i] + M) % M].Grid < Organism[p^1][x][y].Grid &&
                Organism[p][(x + dx[i] + N) % N][(y + dy[i] + M) % M].Grid  == 0)
                    PossDest.emplace_back ( (x + dx[i] + N)%N, (y + dy[i] + M)%M );

    if (PossDest.size() == 0){
        /// ако няма на къде да отиде
        Organism[p][x][y].Grid = Organism[p^1][x][y].Grid;
        Organism[p][x][y].DaysFeed = Organism[p^1][x][y].DaysFeed + 1;
        Organism[p][x][y].DaysLife = Organism[p^1][x][y].DaysLife + 1;

        if (Organism[p][x][y].DaysLife == Breed) Organism[p][x][y].DaysLife = 0;
        return;
    }

    ///Мести се
    std::uniform_int_distribution<> dist(0, PossDest.size()-1);
    int ch = dist(gen);

    int nx = PossDest[ch].first, ny = PossDest[ch].second;

    Organism[p][nx][ny].DaysFeed = Organism[p^1][x][y].DaysFeed + 1;
    Organism[p][nx][ny].DaysLife = Organism[p^1][x][y].DaysLife + 1;
    Organism[p][nx][ny].Grid = Organism[p^1][x][y].Grid;

    ///Reproduce
    if (Organism[p][nx][ny].DaysLife == Breed){
        Organism[p][x][y].DaysFeed = Organism[p][x][y].DaysLife = 0;
        Organism[p][x][y].Grid = Organism[p][nx][ny].Grid;

        Organism[p][nx][ny].DaysLife = 0;
    }
}

int run (int PlanktonBreed, int FishBreed, int FishStarve, int SharkBreed, int SharkStarve, int NumPlankton, int NumFish, int NumShark){
    init (NumPlankton, NumFish, NumShark, PlanktonBreed, FishBreed, SharkBreed, FishStarve, SharkStarve);

    int p = 1;
    for (int st=1; st<=100000; st++){
        memset (Organism[p], 0, sizeof(Organism[p]));

        for (int i=0; i<N; i++)                         ///////Move!
            for (int j=0; j<M; j++)
                if (Organism[p^1][i][j].Grid == 1) move (i, j, p, -1, PlanktonBreed);

        for (int i=0; i<N; i++)
            for (int j=0; j<M; j++)
                if (Organism[p^1][i][j].Grid == 2) move (i, j, p, FishStarve, FishBreed);

        for (int i=0; i<N; i++)
            for (int j=0; j<M; j++)
                if (Organism[p^1][i][j].Grid == 3) move (i, j, p, SharkStarve, SharkBreed);

        int br1 = 0, br2 = 0, br3 = 0;
        for (int i=0; i<N; i++)
            for (int j=0; j<M; j++){
                if (Organism[p][i][j].Grid == 1) br1++;
                if (Organism[p][i][j].Grid == 2) br2++;
                if (Organism[p][i][j].Grid == 3) br3++;
            }

//        print (p);

        if (br1 == 0 || br2 == 0 || br3 == 0) {cout << "FAIL " << st << endl; return 0;}
        p^=1;
    }

    cout << "OK!\n";
}

int main (){
//    clock_t start;
  //  start = clock();

    while (1){
    int a, b, c;
    cin >> a >> b>> c;
    cout <<a << " " << b <<" " <<  c << endl;

    for (int i=0; i<20; i++)
        run (3, 9, 6, 26, 31, a, b, c);
    }

  //  printf ("%lf\n",(double) ((double) clock()-start)/CLOCKS_PER_SEC);
    return 0;
}

/*
 Num:  1193  1034 391
 Breed: 9 14 26
 Starve: 16 22
 Average:  14549  Best: 100000

 Num:  1094  1074 91 <--------------------------------- tova e mnogo qko!
 Breed: 1 11 11
 Starve: 9 30
 Average:  100000  Best: 100000

Num:  1108  851 78
 Breed: 9 26 27
 Starve: 10 28
 Average:  4730  Best: 47511

 Num:  1210  274 53
 Breed: 9 18 28
 Starve: 14 32
 Average:  9260  Best: 25222

Num:  994  964 287
 Breed: 2 9 14
 Starve: 13 30
 Average:  9810  Best: 54843

 Num:  672  597 231
 Breed: 1 15 24
 Starve: 2 27
 Average:  100000  Best: 100000

 Num:  938  860 138
 Breed: 4 17 26
 Starve: 11 29
 Average:  95005  Best: 100000
 Num:  1018  1174 70
 Breed: 3 9 26
 Starve: 6 31
 Average:  100000  Best: 100000
 Num:  1073  849 154
 Breed: 1 8 28
 Starve: 2 15
 Average:  100000  Best: 100000
 Num:  742  789 369
 Breed: 2 9 25
 Starve: 5 23
 Average:  100000  Best: 100000
 Num:  922  1079 47
 Breed: 3 18 25
 Starve: 14 23
 Average:  42666  Best: 100000
 Num:  1139  1081 218
 Breed: 7 19 20
 Starve: 6 31
 Average:  5031  Best: 100000
 Num:  354  1094 39
 Breed: 2 13 16
 Starve: 4 21
 Average:  100000  Best: 100000
 Num:  1156  1027 229
 Breed: 1 9 24
 Starve: 3 23
 Average:  100000  Best: 100000
 Num:  793  780 137
 Breed: 1 9 11
 Starve: 2 32
 Average:  100000  Best: 100000
 Num:  1264  1330 270
 Breed: 1 11 19
 Starve: 2 10
 Average:  100000  Best: 100000
 Num:  1306  866 72
 Breed: 1 17 23
 Starve: 26 28
 Average:  5171  Best: 26905
 Num:  1191  845 206
 Breed: 1 20 24
 Starve: 8 35
 Average:  100000  Best: 100000
 Num:  840  925 400
 Breed: 1 23 24
 Starve: 7 14
 Average:  10071  Best: 100000
 Num:  395  1287 17
 Breed: 1 22 30
 Starve: 4 20
 Average:  55025  Best: 100000
 Num:  845  588 86
 Breed: 5 11 11
 Starve: 5 18
 Average:  100000  Best: 1000000
 Num:  1331  828 38
 Breed: 6 12 21
 Starve: 4 24
 Average:  75015  Best: 100000
 Num:  1223  854 37
 Breed: 1 29 30
 Starve: 22 31
 Average:  100000  Best: 100000
 Num:  1160  228 102
 Breed: 2 18 25
 Starve: 18 24
 Average:  84143  Best: 100000
 Num:  969  571 230
 Breed: 1 10 24
 Starve: 4 11
 Average:  100000  Best: 100000
 Num:  1108  1199 466
 Breed: 5 9 25
 Starve: 12 21
 Average:  5112  Best: 100000
 Num:  407  87 30
 Breed: 2 16 19
 Starve: 6 24
 Average:  100000  Best: 100000
 Num:  1251  552 214
 Breed: 1 19 29
 Starve: 3 29
 Average:  100000  Best: 100000
 Num:  1343  1219 240
 Breed: 3 10 14
 Starve: 3 12
 Average:  70016  Best: 100000
 Num:  619  216 35
 Breed: 14 12 22
 Starve: 4 30
 Average:  19654  Best: 100000
 Num:  868  244 17
 Breed: 4 11 30
 Starve: 9 31
 Average:  100000  Best: 100000
 Num:  1082  369 166
 Breed: 5 19 20
 Starve: 8 24
 Average:  100000  Best: 100000
 Num:  759  482 114
 Breed: 2 9 25
 Starve: 9 18
 Average:  100000  Best: 100000
 Num:  1274  975 38
 Breed: 2 9 24
 Starve: 9 33
 Average:  85025  Best: 100000
 Num:  664  1179 88
 Breed: 4 16 26
 Starve: 17 35
 Average:  9281  Best: 63879
 Num:  893  276 30
 Breed: 12 15 27
 Starve: 9 29
 Average:  27556  Best: 78491
 Num:  781  1025 156
 Breed: 5 15 18
 Starve: 7 23
 Average:  25055  Best: 100000
 Num:  838  1190 306
 Breed: 8 14 27
 Starve: 9 31
 Average:  3041  Best: 60123
 Num:  1235  125 14
 Breed: 2 13 16
 Starve: 5 15
 Average:  95003  Best: 100000
 Num:  1265  397 132
 Breed: 1 15 18
 Starve: 5 9
 Average:  16846  Best: 74284
 Num:  1346  833 178
 Breed: 2 3 13
 Starve: 2 23
 Average:  100000  Best: 100000

 */
