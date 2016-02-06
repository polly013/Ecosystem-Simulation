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
///////////////////////////// това го взех от някъде, ама няма да ви кажа от къде
mt19937 mt_rand(time(0));
mt19937::result_type seed = time(0);

std::mt19937 gen(random_device{}());
std::mt19937 g(random_device{}());
std::uniform_real_distribution<double> Dist (0.0, 1.0);
////////////////////////////

const int MAXN = 128, N = 64, M = 64;
const int Steps = 100000;

int dx[4] = {1, -1, 0, 0};
int dy[4] = {0, 0, 1, -1};

int avgBest = 0, bestBest = 0;

struct structOrganisms{
    int Grid;
    double Reproduce, Starve;
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

    int i = 0;
    for (; i<NumPlankton; i++){
        Organism[p][coord[i].first][coord[i].second].Grid = 1;
        Organism[p][coord[i].first][coord[i].second].Reproduce = (double) 1/PlanktonBreed;
        Organism[p][coord[i].first][coord[i].second].Starve = -1.0;
    }

    for (; i<NumPlankton+NumFish; i++){
        Organism[p][coord[i].first][coord[i].second].Grid = 2;
        Organism[p][coord[i].first][coord[i].second].Reproduce = (double) 1/FishBreed;
        Organism[p][coord[i].first][coord[i].second].Starve = (double) 1/FishStarve;
    }

    for (; i<NumPlankton+NumFish+NumShark; i++){
        Organism[p][coord[i].first][coord[i].second].Grid = 3;
        Organism[p][coord[i].first][coord[i].second].Reproduce = (double) 1/SharkBreed;
        Organism[p][coord[i].first][coord[i].second].Starve = (double) 1/SharkStarve;
    }
}

bool eat (int a, int b, int p, int predator){ /// Eat and Breed
    vector<pair<int,int> > Food;

    for (int i=0; i<4; i++)
        if (Organism[p][(a + dx[i] + N) % N][(b + dy[i] + M) % M].Grid == predator-1)
            Food.emplace_back ( (a+dx[i]+N)%N, (b+dy[i]+M)%M );

    if (Food.size () == 0) /// There isn't any food around :(
        return 0;

    std::uniform_int_distribution<> dist(0, Food.size()-1);

    int ch = dist(gen);
    int nx = Food[ch].first, ny = Food[ch].second;

    Organism[p][nx][ny].Grid = predator;
    Organism[p][nx][ny].Reproduce = Organism[p^1][a][b].Reproduce;
    Organism[p][nx][ny].Starve = Organism[p^1][a][b].Starve;

//    cout << a << " " << b << "   манджа ^^  " << nx << " " << ny << endl;

    ///////// Reproduce
    double ProbabilityValue = Dist (gen);

    if (ProbabilityValue < Organism[p][nx][ny].Reproduce) {
        Organism[p][a][b] = Organism[p][nx][ny];
     //   cout << "въй, репродуцирам се ^^\n";
    }

    return 1;
}

void move (int x, int y, int p){ // местим организма, стоящ в координати (x, y)
    /////////Яде
    if (Organism[p^1][x][y].Grid != 1)
        if (eat (x, y, p, Organism[p^1][x][y].Grid)) return;

    ////////Ако няма да яде
//    cout << x << " " <<  y << " no food for me :C\n";
    double ProbabilityValue = Dist (gen);

    if (ProbabilityValue < Organism[p^1][x][y].Starve) {
        Organism[p^1][x][y].Grid = 0; Organism[p^1][x][y].Starve = 0; Organism[p^1][x][y].Reproduce = 0;
      //  cout << "умрях :C\n";
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
    //    cout << " не мърдам\n";
        /// ако няма на къде да отиде
        Organism[p][x][y] = Organism[p^1][x][y];
        return;
    }


    ///Мести се
    std::uniform_int_distribution<> dist(0, PossDest.size()-1);
    int ch = dist(gen);

    int nx = PossDest[ch].first, ny = PossDest[ch].second;

    Organism[p][nx][ny] = Organism[p^1][x][y];
  //  cout << "мястя се в " << nx << " " << ny << endl;

    ///Reproduce
    ProbabilityValue = Dist (gen);

    if (ProbabilityValue < Organism[p][nx][ny].Reproduce) {
        Organism[p][x][y]= Organism[p][nx][ny];
//        cout << "ВЪЙ, ВЪЙ, ВЪЙ\n";
    }
}

int run (int PlanktonBreed, int FishBreed, int FishStarve, int SharkBreed, int SharkStarve, int NumPlankton, int NumFish, int NumShark){
    init (NumPlankton, NumFish, NumShark, PlanktonBreed, FishBreed, SharkBreed, FishStarve, SharkStarve);

    vector<int> vec[2];
    int p = 1;
    for (int st=1; st<=100000; st++){
        memset (Organism[p], 0, sizeof(Organism[p]));

        for (int i=0; i<N; i++)                         ///////Move!
            for (int j=0; j<M; j++)
                if (Organism[p^1][i][j].Grid == 1) move (i, j, p);

        for (int i=0; i<N; i++)
            for (int j=0; j<M; j++)
                if (Organism[p^1][i][j].Grid == 2) move (i, j, p);

        for (int i=0; i<N; i++)
            for (int j=0; j<M; j++)
                if (Organism[p^1][i][j].Grid == 3) move (i, j, p);

        int br1 = 0, br2 = 0, br3 = 0;
        for (int i=0; i<N; i++)
            for (int j=0; j<M; j++){
                if (Organism[p][i][j].Grid == 1) br1++;
                if (Organism[p][i][j].Grid == 2) br2++;
                if (Organism[p][i][j].Grid == 3) br3++;
            }

    //    print (p);

//        cout << br1 << " " << br2 << " " << br3 << "\n";
        if (br1 == 0 || br2 == 0 || br3 == 0) {cout << "FAIL " << st << endl; return 0;}
        p^=1;
    }

    return 1;
}

int main (){
    run (1, 11, 9, 11, 30, 1094, 1074, 91);
//    run (3, 9, 6, 26, 31, 1018, 1174, 70);
    return 0;
}

