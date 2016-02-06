#include<iostream>
#include<cstring>
#include<algorithm>
#include<cstdio>
#include<vector>
#include<cstdlib>
#include <random>
#include <iterator>
#include<functional>
#include<vector>
using namespace std;
//////////////////////////////////////////////////////////////////////////////////////
random_device rd;
mt19937 g(rd());
mt19937 mt_rand(time(0));
mt19937::result_type seed = time(0);

std::mt19937 gen(random_device{}());
///////////////////////////////////////////////////////////////////////////////////////

const int MAXN = 128, N = 64, M = 64;
const int Steps = 100000;
const int MAXPopulation = 800, MAXP = 80;
const int Elit = 7*MAXPopulation/100, MutationRate = 5;
const int AVG = 20;


int F[MAXPopulation];

int dx[4] = {1, -1, 0, 0};
int dy[4] = {0, 0, 1, -1};

int avgBest = 0, bestBest = 0;
pair<int,int> El[MAXPopulation];

struct G{
    int NumPlankton, NumFish, NumShark;
    int PlanktonBreed, FishBreed, SharkBreed;
    int FishStarve, SharkStarve;
}Genome[2][MAXPopulation], Best;

struct structOrganisms{
    int Grid, DaysFeed, DaysLife;
}Organism[2][MAXN][MAXN];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::uniform_int_distribution<> numorg(1, N*M/3);
std::uniform_int_distribution<> breed(1,30);
std::uniform_int_distribution<> die (2, 35);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void init (int NumPlankton, int NumFish, int NumShark){
    int p = 0;
    memset (Organism[p], 0, sizeof(Organism[p]));

    vector<pair<int, int> > coord;
    for (int i=0; i<N; i++)
        for (int j=0; j<M; j++)
            coord.push_back (make_pair(i,j));

    shuffle(coord.begin(), coord.end(), g);

    int i = 0;
    for (; i<NumPlankton; i++)
        Organism[p][coord[i].first][coord[i].second].Grid = 1;

    for (; i<NumPlankton+NumFish; i++)
        Organism[p][coord[i].first][coord[i].second].Grid = 2;

    for (; i<NumPlankton+NumFish+NumShark; i++)
        Organism[p][coord[i].first][coord[i].second].Grid = 3;
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
//    Organism[p][nx][ny].DontMove = -1;

//    cout << "манджа ^^  " << nx << " " << ny << endl;
    if (Organism[p][nx][ny].DaysLife == Breed) { ///Reproduce
 //       cout << "репродуцирам се\n";
        Organism[p][a][b].Grid = pred;
        Organism[p][a][b].DaysFeed = Organism[p][a][b].DaysLife = 0;
//        Organism[p][a][b].DontMove = p;

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
        Organism[p][x][y].Grid = 0; Organism[p][x][y].DaysFeed = 0; Organism[p][x][y].DaysLife = 0; //Organism[p][x][y].DontMove = -1;
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
//        Organism[p][x][y].DontMove = -1;

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
  //  Organism[p][nx][ny].DontMove = -1;

    if (Organism[p][nx][ny].DaysLife == Breed){ ///Reproduce
//        cout << "репродуцирам се\n";
        Organism[p][x][y].DaysFeed = Organism[p][x][y].DaysLife = 0;
        Organism[p][x][y].Grid = Organism[p][nx][ny].Grid;
//        Organism[p][x][y].DontMove = p;

        Organism[p][nx][ny].DaysLife = 0;
    }
  //  cout << "преместих се   " << nx << " " << ny << endl;
}

int run (int PlanktonBreed, int FishBreed, int FishStarve, int SharkBreed, int SharkStarve, int NumPlankton, int NumFish, int NumShark){
    init (NumPlankton, NumFish, NumShark);

    int p = 1;
    for (int st=1; ; st++){
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

//        cout << br1 << ", " << br2 << ", " << br3 << ";\n";
        if (br1 == 0 || br2 == 0 || br3 == 0) return st;
        if (st == Steps) return st;
        p^=1;
    }
}

void Mutation (int p, int t){
    std::uniform_int_distribution<> change(0, 7);
    int ch = change (gen);

    if (ch == 0) Genome[p][t].PlanktonBreed = breed(gen);
    if (ch == 1) Genome[p][t].FishBreed =     breed(gen);
    if (ch == 2) Genome[p][t].SharkBreed =    breed(gen);

    if (ch == 3) Genome[p][t].NumPlankton = numorg(gen);
    if (ch == 4) Genome[p][t].NumFish =     numorg(gen);
    if (ch == 5) Genome[p][t].NumShark =    numorg(gen);

    if (ch == 6) Genome[p][t].FishStarve  = die(gen);
    if (ch == 7) Genome[p][t].SharkStarve = die(gen);
}

void Crossover (int p, int t, int a, int b){
    Genome[p][t].NumPlankton = (Genome[p^1][a].NumPlankton+Genome[p^1][b].NumPlankton+1)/2;
    Genome[p][t].NumFish =     (Genome[p^1][a].NumFish+Genome[p^1][b].NumFish+1)/2;
    Genome[p][t].NumShark =    (Genome[p^1][a].NumShark+Genome[p^1][b].NumShark+1)/2;

    Genome[p][t].PlanktonBreed = (Genome[p^1][a].PlanktonBreed + Genome[p^1][b].PlanktonBreed+1)/2;
    Genome[p][t].FishBreed =     (Genome[p^1][a].FishBreed + Genome[p^1][b].FishBreed+1)/2;
    Genome[p][t].SharkBreed =    (Genome[p^1][a].SharkBreed + Genome[p^1][b].SharkBreed+1)/2;

    Genome[p][t].FishStarve =  (Genome[p^1][a].FishStarve + Genome[p^1][b].FishStarve+1)/2;
    Genome[p][t].SharkStarve = (Genome[p^1][a].SharkStarve + Genome[p^1][b].SharkStarve+1)/2;
}

void FitnessFunction (int p, int t){
    int average = 0, currBest = 0;
    for (int i=0; i<AVG; i++){
        int current = run (Genome[p][t].PlanktonBreed, Genome[p][t].FishBreed, Genome[p][t].FishStarve,
                        Genome[p][t].SharkBreed, Genome[p][t].SharkStarve,
                        Genome[p][t].NumPlankton, Genome[p][t].NumFish, Genome[p][t].NumShark);

        average += current;
        if (currBest < current) currBest = current;
    }

    F[t] = average;
    El[t].first = F[t];
    El[t].second = t;

    if (average > avgBest){
        avgBest = average;
        bestBest = currBest;
        Best = Genome[p][t];

        if (bestBest >= 20000){
            cout << " Num:  " << Best.NumPlankton << "  " << Best.NumFish << " " << Best.NumShark << endl;
            cout << " Breed: " << Best.PlanktonBreed << " " << Best.FishBreed << " " << Best.SharkBreed << endl;
            cout << " Starve: " << Best.FishStarve << " " << Best.SharkStarve << endl;
            cout << " Average:  " << avgBest/AVG << "  Best: " << bestBest << endl;
        }
    }
    else if (currBest >= 25000){
        cout << " Num:  " << Genome[p][t].NumPlankton << "  " << Genome[p][t].NumFish << " " << Genome[p][t].NumShark << endl;
        cout << " Breed: " << Genome[p][t].PlanktonBreed << " " << Genome[p][t].FishBreed << " " << Genome[p][t].SharkBreed << endl;
        cout << " Starve: " << Genome[p][t].FishStarve << " " << Genome[p][t].SharkStarve << endl;
        cout << " Average:  " << average/AVG << "  Best: " << currBest << endl;
    }
}

int Selection (){
    long long sum = 0;
    for (int i=0; i<MAXPopulation; i++)
        sum += F[i];

    if (sum < 0) cout << "NO\n";
    if (sum == 0) return 0;

    std::uniform_int_distribution<> choose(0, sum);
    long long r = choose(gen);
    long long currsum = F[0];

    if (r < currsum) return 0;

    for (int i=1; i<MAXPopulation; i++){
        if (r < currsum+F[i]) return i;
        currsum += F[i];
    }

    cout << r << " " << currsum+F[MAXPopulation-1] << " " << sum << endl;
    cout <<"HAHA\n";
    return 0;
}

void Elitism (int p){
    sort (El, El+MAXPopulation);

    for (int i=0; i<Elit; i++)
        Genome[p^1][i] = Genome[p][El[i].second];
}

void InitialPopulation (){
    for (int i=0; i<MAXPopulation; i++){
        Genome[0][i].NumShark = numorg(gen);
        Genome[0][i].NumFish = numorg(gen);
        Genome[0][i].NumPlankton = numorg(gen);

        if (Genome[0][i].NumPlankton < Genome[0][i].NumFish) swap (Genome[0][i].NumPlankton, Genome[0][i].NumFish);
        if (Genome[0][i].NumFish < Genome[0][i].NumShark) swap (Genome[0][i].NumShark, Genome[0][i].NumFish);
        Genome[0][i].NumShark /= 2;

        Genome[0][i].FishBreed = breed(gen);
        Genome[0][i].PlanktonBreed = breed(gen);
        Genome[0][i].SharkBreed = breed(gen);

        if (Genome[0][i].FishBreed < Genome[0][i].PlanktonBreed) swap (Genome[0][i].PlanktonBreed, Genome[0][i].FishBreed);
        if (Genome[0][i].SharkBreed < Genome[0][i].FishBreed) swap (Genome[0][i].FishBreed, Genome[0][i].SharkBreed);

        Genome[0][i].FishStarve = die (gen);
        Genome[0][i].SharkStarve = die (gen);

        if (Genome[0][i].SharkStarve < Genome[0][i].FishStarve) swap (Genome[0][i].FishStarve, Genome[0][i].SharkStarve);

        FitnessFunction (0, i);
    }
}

void GeneticAlgorithm (){
    InitialPopulation();
    int p = 1;

    for (int i=0; i<MAXP; i++){
        Elitism (p^1);

        for (int t=Elit; t<MAXPopulation; t++){
            int a = Selection();
            int b = Selection();

            if (a == b) {
                std::uniform_int_distribution <> newb (0, MAXPopulation);
                b = newb(gen);
                if (a == b) b++;
                if (b == MAXPopulation) b = 0;
            }

            Crossover (p, t, a, b);

            Mutation (p, t);
            FitnessFunction(p, t);
        }
    }

    cout << " Num:  " << Best.NumPlankton << "  " << Best.NumFish << " " << Best.NumShark << endl;
    cout << " Breed: " << Best.PlanktonBreed << " " << Best.FishBreed << " " << Best.SharkBreed << endl;
    cout << " Starve: " << Best.FishStarve << " " << Best.SharkStarve << endl;
    cout << " Average:  " << avgBest/AVG << "  Best: " << bestBest << endl;
}

int main (){
    GeneticAlgorithm ();
    return 0;
}

