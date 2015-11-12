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
#include<queue>
///#include<bitset>
using namespace std;
//////////////////////////////
random_device rd;
mt19937 g(rd());
mt19937 mt_rand(time(0));
mt19937::result_type seed = time(0);

std::mt19937 gen(random_device{}());
//////////////////////////////


//const int MAXPlankton = 5000, MAXFish = 1200, MAXShark = 600;
const int MAXPlankton = 4000, MAXFish = 500, MAXShark = 400;
const int plankton_tr = 100, fish_tr = 50, shark_tr = 20;
const int PlanktonBreed = 3, FishBreed = 9, SharkBreed = 26, FishStarve = 6, SharkStarve = 31;

bool used[MAXPlankton][MAXFish][MAXShark];

const int trpl[6] = {2*plankton_tr+1, -2*plankton_tr-1, 0, 0, 0, 0};
const int trfi[6] = {0, 0, 2*fish_tr+1, -2*fish_tr-1, 0, 0};
const int trsh[6] = {0, 0, 0, 0, 2*shark_tr+1, -2*shark_tr-1};

struct Triple{
    int plankton, fish, shark;
};

queue<Triple> q;

/////////////////////////////////////
const int MAXN = 128, N = 64, M = 64;
const int Steps = 10000;

const int dx[4] = {1, -1, 0, 0};
const int dy[4] = {0, 0, 1, -1};

struct structOrganisms{
    int Grid, DaysFeed, DaysLife;
}Organism[2][MAXN][MAXN];
////////////////////////////////////

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
  //  cout << "eat  " << a << " " << b << endl;
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

//    cout << "манджа ^^  " << nx << " " << ny << endl;
    if (Organism[p][nx][ny].DaysLife == Breed) { ///Reproduce
 //       cout << "репродуцирам се\n";
        Organism[p][a][b].Grid = pred;
        Organism[p][a][b].DaysFeed = Organism[p][a][b].DaysLife = 0;

        Organism[p][nx][ny].DaysLife = 0;
    }
    else Organism[p][a][b].Grid = 0;

    return 1;
}

void move (int x, int y, int p, int Starve, int Breed){ // местим организма, стоящ в координати (i,j)
  //  cout << x << " " << y << endl;
    /////////Яде
    if (Organism[p^1][x][y].Grid != 1)
        if (eat (x, y, p, Organism[p^1][x][y].Grid, Breed)) return;

    ////////Ако няма да яде
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
//        cout << "репродуцирам се\n";
        Organism[p][x][y].DaysFeed = Organism[p][x][y].DaysLife = 0;
        Organism[p][x][y].Grid = Organism[p][nx][ny].Grid;

        Organism[p][nx][ny].DaysLife = 0;
    }
  //  cout << "преместих се   " << nx << " " << ny << endl;
}

int run (int PlanktonBreed, int FishBreed, int SharkBreed, int FishStarve, int SharkStarve, int NumPlankton, int NumFish, int NumShark){
    init (NumPlankton, NumFish, NumShark, PlanktonBreed, FishBreed, SharkBreed, FishStarve, SharkStarve);

    int p = 1;
    for (int st=1; st<=Steps; st++){
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

//        cout << br1 << " " << br2 << " " << br3 << "\n";
        if (br1 == 0) return 1;
        if (br2 == 0) return 2;
        if (br3 == 0) return 3;
        p^=1;
    }

    return 4;
}

int main (){
    cout <<sizeof(used) << endl;
    Triple starting_point;
    starting_point.plankton = 3107;
    starting_point.fish = 296;
    starting_point.shark = 155;

    used[3107][296][155] = 1;
    q.push (starting_point);

    while (!q.empty()){
        Triple current = q.front();
        q.pop();

        int b[5] = {0,0,0,0,0};

        for (int i=0; i<20; i++){
            int balance = run (PlanktonBreed, FishBreed, SharkBreed, FishStarve, SharkStarve, current.plankton, current.fish, current.shark);
            cout << balance << " " ;
            b[balance]++;
        }
        cout << endl;

        Triple help;

        if (b[1] == 20)
            for (int i=0; i<6; i++){
                help.plankton = current.plankton + trpl[i];
                help.fish = current.fish + trfi[i];
                help.shark = current.shark + trsh[i];

                if (help.plankton <= current.plankton && help.fish >= current.fish) continue;
                if (help.plankton+help.fish+help.shark > N*N) continue;

                if (help.plankton > 0 && help.fish > 0 && help.shark > 0 &&
                        help.plankton < MAXPlankton && help.fish < MAXFish && help.shark < MAXShark)
                    if (used[help.plankton][help.fish][help.shark] == 0) {
                        used[help.plankton][help.fish][help.shark] = 1;
                        q.push (help);
                    }
            }

        else if (b[2] == 20)
            for (int i=0; i<6; i++){
                help.plankton = current.plankton + trpl[i];
                help.fish = current.fish + trfi[i];
                help.shark = current.shark + trsh[i];

                if (help.plankton <= current.plankton && help.fish == current.fish) continue;
                if (help.fish <= current.fish && help.shark >= current.shark) continue;

                if (help.plankton+help.fish+help.shark > N*N) continue;

                if (help.plankton > 0 && help.fish > 0 && help.shark > 0 &&
                        help.plankton < MAXPlankton && help.fish < MAXFish && help.shark < MAXShark)
                    if (used[help.plankton][help.fish][help.shark] == 0) {
                        used[help.plankton][help.fish][help.shark] = 1;
                        q.push (help);
                    }
            }

        else if (b[3] == 20)
            for (int i=0; i<6; i++){
                help.plankton = current.plankton + trpl[i];
                help.fish = current.fish + trfi[i];
                help.shark = current.shark + trsh[i];

                if (help.fish <= current.fish && help.shark <= current.shark) continue;
                if (help.plankton+help.fish+help.shark > N*N) continue;

                if (help.plankton > 0 && help.fish > 0 && help.shark > 0 &&
                        help.plankton < MAXPlankton && help.fish < MAXFish && help.shark < MAXShark)
                    if (used[help.plankton][help.fish][help.shark] == 0) {
                        used[help.plankton][help.fish][help.shark] = 1;
                        q.push (help);
                    }
            }

        else{
            for (int i=0; i<6; i++){
                help.plankton = current.plankton + trpl[i];
                help.fish = current.fish + trfi[i];
                help.shark = current.shark + trsh[i];

                if (help.plankton+help.fish+help.shark > N*N) continue;

                if (help.plankton > 0 && help.fish > 0 && help.shark > 0 &&
                        help.plankton < MAXPlankton && help.fish < MAXFish && help.shark < MAXShark)
                    if (used[help.plankton][help.fish][help.shark] == 0) {
                        used[help.plankton][help.fish][help.shark] = 1;
                        q.push (help);
                    }
            }
            cout << current.plankton <<  " "<< current.fish << " " << current.shark << endl;
        }
    }
    return 0;
}
