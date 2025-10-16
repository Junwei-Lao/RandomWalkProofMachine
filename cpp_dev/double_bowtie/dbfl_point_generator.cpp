#include <iostream>
#include <random>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>

//------------------------------------------------------------structure------------------------------------------------------------------------//
struct tuple{
    int x;
    int y;
    
    tuple(int xInput, int yInput) {
        this->x = xInput;
        this->y = yInput;
    }
};

struct node{
    int x;
    int y;
    node* nextNode;

    node(int xInput, int yInput) {
        this->x = xInput;
        this->y = yInput;
        this->nextNode = nullptr;
    }

    node(tuple t) {
        this->x = t.x;
        this->y = t.y;
        this->nextNode = nullptr;
    }

};



//--------------------------------------------------------------functions------------------------------------------------------------------------//
bool comparator(node* first, node* second) {
    return (first->x == second->x && first->y == second->y);
}

int hashCode (tuple first, int HashSize) {  //new hash function
    uint64_t hx = (uint64_t)(uint32_t)first.x;
    uint64_t hy = (uint64_t)(uint32_t)first.y;
    return (hx << 32 | hy) % HashSize;
}

bool Find_And_Insert_Node(node** hashmap, tuple t, int HashSize) {
    int code = hashCode(t, HashSize);
    if (hashmap[code] == nullptr) {
        hashmap[code] = new node(t);
        return true;
    } else {
        node* tempoNode = hashmap[code];
        node* newNode = new node(t);

        while (true) {
            if (comparator(tempoNode, newNode)) {
                delete newNode; 
                return false;
            }

            if (tempoNode->nextNode != nullptr) {
                tempoNode = tempoNode->nextNode;
            } else {
                tempoNode->nextNode = newNode;
                return true;
            }
        }
    }
}



//--------------------------------------------------------------constant variables-------------------------------------------------------------//
double width_list[] = {10.040429153935227, 10.304650973775628, 10.568872793616029, 10.83309461345643, 11.09731643329683, 11.361538253137232, 11.62576007297763, 11.889981892818032, 12.154203712658433, 12.418425532498834, 12.682647352339234, 12.946869172179635, 13.211090992020036, 13.475312811860437, 13.739534631700838, 14.003756451541237, 14.267978271381638, 14.53220009122204, 14.79642191106244, 15.060643730902841, 15.324865550743242, 15.589087370583643, 15.853309190424042, 16.117531010264443, 16.381752830104844, 16.645974649945245, 16.910196469785646, 17.174418289626047, 17.438640109466448, 17.70286192930685, 17.96708374914725, 18.23130556898765, 18.49552738882805, 18.759749208668453, 19.023971028508853, 19.28819284834925, 19.552414668189652, 19.816636488030053, 20.080858307870454, 20.345080127710855, 20.609301947551256, 20.873523767391656, 21.137745587232057, 21.40196740707246, 21.66618922691286, 21.93041104675326, 22.19463286659366, 22.458854686434062, 22.723076506274463, 22.987298326114864, 23.25152014595526, 23.515741965795662, 23.779963785636063, 24.044185605476464, 24.308407425316865, 24.572629245157266, 24.836851064997667, 25.101072884838068, 25.36529470467847, 25.62951652451887, 25.89373834435927, 26.15796016419967, 26.422181984040073, 26.686403803880474, 26.950625623720875, 27.214847443561275, 27.479069263401676, 27.743291083242074, 28.007512903082475, 28.271734722922876, 28.535956542763277, 28.800178362603678, 29.06440018244408, 29.32862200228448, 29.59284382212488, 29.85706564196528, 30.121287461805682, 30.385509281646083, 30.649731101486484, 30.913952921326885, 31.178174741167286, 31.442396561007687, 31.706618380848084, 31.970840200688485, 32.235062020528886, 32.49928384036929, 32.76350566020969, 33.02772748005009, 33.29194929989049, 33.55617111973089, 33.82039293957129, 34.08461475941169, 34.348836579252094, 34.613058399092495, 34.877280218932896, 35.1415020387733, 35.4057238586137, 35.6699456784541, 35.9341674982945, 36.1983893181349, 36.4626111379753, 36.7268329578157, 36.9910547776561, 37.255276597496504, 37.519498417336905, 37.783720237177306, 38.04794205701771, 38.3121638768581, 38.5763856966985, 38.8406075165389, 39.104829336379304, 39.369051156219705, 39.633272976060105, 39.897494795900506, 40.16171661574091, 40.42593843558131, 40.69016025542171, 40.95438207526211, 41.21860389510251, 41.48282571494291, 41.74704753478331, 42.011269354623714, 42.275491174464115, 42.539712994304516, 42.80393481414492, 43.06815663398532, 43.33237845382572, 43.59660027366612, 43.86082209350652, 44.12504391334692, 44.38926573318732, 44.65348755302772, 44.917709372868124, 45.181931192708525, 45.446153012548926, 45.71037483238933, 45.97459665222973, 46.23881847207013, 46.50304029191052, 46.767262111750924, 47.031483931591325, 47.295705751431726, 47.55992757127213, 47.82414939111253, 48.08837121095293, 48.35259303079333, 48.61681485063373, 48.88103667047413, 49.14525849031453, 49.40948031015493, 49.673702129995334, 49.937923949835735, 50.202145769676136, 50.46636758951654, 50.73058940935694, 50.99481122919734, 51.25903304903774, 51.52325486887814, 51.78747668871854, 52.05169850855894, 52.31592032839934, 52.580142148239744, 52.844363968080145, 53.108585787920546, 53.37280760776095, 53.63702942760135, 53.90125124744175, 54.16547306728215, 54.42969488712255, 54.69391670696295, 54.95813852680335, 55.22236034664375, 55.48658216648415, 55.75080398632455, 56.01502580616495, 56.27924762600535, 56.54346944584575, 56.80769126568615, 57.07191308552655, 57.336134905366954, 57.600356725207355, 57.864578545047756, 58.12880036488816, 58.39302218472856, 58.65724400456896, 58.92146582440936, 59.18568764424976, 59.44990946409016, 59.71413128393056, 59.978353103770964, 60.242574923611365, 60.506796743451766, 60.77101856329217, 61.03524038313257, 61.29946220297297, 61.56368402281337, 61.82790584265377, 62.09212766249417, 62.35634948233457, 62.62057130217497, 62.884793122015374, 63.149014941855775, 63.41323676169617, 63.67745858153657, 63.94168040137697, 64.20590222121737, 64.47012404105777, 64.73434586089817, 64.99856768073857, 65.26278950057898, 65.52701132041938, 65.79123314025978, 66.05545496010018, 66.31967677994058, 66.58389859978098, 66.84812041962138, 67.11234223946178, 67.37656405930218, 67.64078587914258, 67.90500769898298, 68.16922951882339, 68.43345133866379, 68.69767315850419, 68.96189497834459, 69.22611679818499, 69.49033861802539, 69.75456043786579, 70.01878225770619, 70.2830040775466, 70.547225897387, 70.8114477172274, 71.0756695370678, 71.3398913569082, 71.6041131767486, 71.868334996589, 72.1325568164294, 72.3967786362698, 72.6610004561102, 72.9252222759506, 73.189444095791, 73.4536659156314, 73.7178877354718, 73.9821095553122, 74.24633137515261, 74.51055319499301, 74.77477501483341, 75.03899683467381, 75.30321865451421, 75.56744047435461, 75.83166229419501, 76.09588411403541, 76.3601059338758, 76.6243277537162, 76.8885495735566, 77.152771393397, 77.4169932132374, 77.6812150330778, 77.9454368529182, 78.20965867275861, 78.47388049259901, 78.73810231243941, 79.00232413227981};
//Need change ↓
double point_list[] = {15441, 16010, 16625, 17249, 17881, 18612, 19396, 19880, 20684, 21360, 22004, 22696, 23396, 24104, 24930, 25809, 26351, 27209, 27961, 28722, 29489, 30225, 31049, 31970, 32864, 33542, 34496, 35324, 36160, 37004, 37816, 38716, 39732, 40531, 41457, 42505, 43409, 44322, 45241, 46129, 47066, 48216, 49090, 50092, 51236, 52216, 53382, 54160, 55164, 56360, 57609, 58373, 59641, 60689, 61745, 63006, 63841, 64921, 66212, 67556, 68376, 69740, 70864, 71956, 73096, 74244, 75400, 76786, 78225, 79103, 80521, 81761, 82930, 84145, 85329, 86601, 88082, 89576, 90550, 92064, 93340, 94624, 95916, 97176, 98524, 100100, 101650, 102721, 104329, 105681, 107042, 108409, 109745, 111130, 113120, 114162, 115612, 117316, 118744, 120470, 121624, 123036, 124792, 126601, 127701, 129529, 131025, 132529, 134350, 135521, 137049, 138900, 140804, 141960, 143884, 145456, 146996, 148952, 150180, 151784, 153730, 155729, 156943, 158961, 160609, 162226, 163889, 165521, 167241, 169636, 171336, 172646, 174720, 176444, 178176, 179916, 181624, 183420, 185929, 187666, 189073, 191241, 193041, 194850, 196665, 198449, 200321, 202944, 204756, 206220, 208484, 210360, 212244, 214136, 215996, 218312, 220681, 222117, 224505, 226449, 228401, 230782, 232289, 234265, 236676, 239140, 240632, 243116, 245136, 247164, 249640, 251204, 253256, 256209, 258321, 259871, 262449, 264545, 266610, 269220, 270841, 272969, 276036, 278184, 279830, 282464, 284636, 286816, 289004, 291160, 293404, 296585, 298809, 300513, 303241, 305489, 307746, 310009, 312241, 314561, 317856, 320116, 321916, 324740, 327064, 329396, 331736, 334044, 336920, 339849, 342186, 344569, 346961, 349361, 352302, 354185, 356569, 360080, 362564, 364976, 367436, 369904, 372380, 375416, 377316, 379816, 383441, 386001, 387887, 391025, 393569, 396082, 399252, 401209, 403785, 407524, 410120, 412102, 415336, 417916, 420544, 423810, 425784, 428476, 432329, 435001, 437041, 440329, 443025, 445730, 449090, 451121, 453889, 457856, 460564, 462700, 466084, 468856, 471636, 474424, 477180, 480657};

const int sample_size = 100; //for random walk
const tuple directions[4] {tuple(-1, 0), tuple(1, 0), tuple(0, -1), tuple(0, 1)};

const double fixed_width = 40.0;

const int elementMoves = 0;
//--------------------------------------------------------------main function-----------------------------------------------------------------//
int main() {
    
    std::random_device rd;
    std::mt19937 gen(rd());


    for (int element = 0; element < 300-38; element++) {

        if (element != 60 + elementMoves) {
            continue;
        }

        const double width = width_list[element];
        const double hypo_side = 4*width;
        const double length = 5*width;
        const int totalPoints = point_list[element];

        int totalPoints_half = totalPoints / 2; //for random walk
        const double Long = 2 * hypo_side + length;
        const double Height = 2 * hypo_side + fixed_width;

        const double gate = 0.25 * length + hypo_side;
        const double gate2 = 0.5 * length + hypo_side;
        const double gate3 = 0.75 * length + hypo_side; //for random walk

        const double vertical_bottom = (fixed_width - length)/2;
        const double vertical_top = vertical_bottom + Long;


        const double a = hypo_side + length/2;
        const double b = hypo_side + fixed_width/2;

        const double box_left = a - fixed_width/2;
        const double box_right = a + fixed_width/2;
        const double box_up = b + fixed_width/2;
        const double box_bottom = b - fixed_width/2;


        const int hashSize = 2 * totalPoints;

        std::uniform_int_distribution<> randomX(0, Long+2);
        std::uniform_int_distribution<> randomY(vertical_bottom - 2, vertical_top + 2);

        for (int overall_count = 0; overall_count < sample_size; overall_count++) {

            std::cout << "Element: " << element << ", Sample: " << overall_count << std::endl;

            //------------------------------------------initialize-------------------------------------------//
            tuple machine(0, 0);
            int gateCrossedCount = 0;
            int gateCrossedCount2 = 0;
            int gateCrossedCount3 = 0;

            int gateCrossedCount_vertical = 0;
            int gateCrossedCount2_vertical = 0;
            int gateCrossedCount3_vertical = 0;

            bool isLeft = false;



            std::ostringstream filename_stream;

            // Build the filename using the string stream
            filename_stream << "walks/walk" << overall_count << ".txt";

            // Get the final string from the stream
            std::string filename = filename_stream.str();

            std::ofstream out(filename);

            out << 0 << " " << 0 << " " << hypo_side << " " << box_left << " " << box_left << " " << box_left-hypo_side << " " << box_right + hypo_side << " " << box_right << " " << box_right << " " << hypo_side + length << " " << Long << " " << Long << " " << hypo_side + length << " " << box_right << " " << box_right << " " << box_right + hypo_side << " " << box_left - hypo_side << " " << box_left << " " << box_left << " " << hypo_side << " " << 0 << std::endl; 
            out << 0 << " " << Height << " " << hypo_side + fixed_width << " " << hypo_side + fixed_width << " " << b + length/2 << " " << vertical_top << " " << vertical_top << " " << b + length/2 << " " << hypo_side + fixed_width << " " << hypo_side + fixed_width << " " << Height << " " << 0 << " " << hypo_side << " " << hypo_side << " " << b - length/2 << " " << vertical_bottom << " " << vertical_bottom << " " << b - length/2 << " " << hypo_side << " " << hypo_side << " " << 0 << std::endl;
            out << gate << " " << gate << std::endl;
            out << hypo_side << " " << hypo_side + fixed_width << std::endl;
            out << box_left << " " << box_right << " " << box_right << " " << box_left << " " << box_left << std::endl;
            out << box_bottom << " " << box_bottom << " " << box_up << " " << box_up << " " << box_bottom << std::endl;
            out << gate3 << " " << gate3 << std::endl;
            out << hypo_side << " " << hypo_side + fixed_width << std::endl;
            
            while (true) {
                int machineX = randomX(gen);
                int machineY = randomY(gen);

                if (machineX == gate || machineX == gate2 || machineX == gate3) {
                    continue; // Skip if the machine is at the gate
                }

                if (machineX <= hypo_side){
                    if (machineY >= machineX && machineY <= Height - machineX){
                        machine = tuple(machineX, machineY);
                        isLeft = true;
                        break;
                    }
                } 
                
            }

            int walk_count = 0;
            int unique_points = 1;
            node* path[hashSize] = {};
            Find_And_Insert_Node(path, machine, hashSize);
            //--------------------------------------------------------------------------------------------------------//

            //------------------------------------------random walk---------------------------------------------------//
            bool nextLeft, nextRight, originLeft, originRight;
            bool keep = false;

            bool nextLeft2, nextRight2, originLeft2, originRight2;
            bool keep2 = false;

            bool nextLeft3, nextRight3, originLeft3, originRight3;
            bool keep3 = false;


            int finalCrossGate1 = 0;
            int finalCrossGate2 = 0;
            int finalCrossGate3 = 0;

            
            while (unique_points < totalPoints_half) {
                std::vector<tuple> DirectionChoice;


                
                out << machine.x << " " << machine.y << " " << walk_count << " " << gateCrossedCount << " " << finalCrossGate1 << " " << gateCrossedCount2 << " " << finalCrossGate2 << " " << gateCrossedCount3 << " " << finalCrossGate3 << " " << unique_points << std::endl;




                if (!keep) {
                    if (machine.x < gate && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                        originLeft = true;
                        originRight = false;
                    } else if (machine.x > gate && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                        originLeft = false;
                        originRight = true;
                    } else {
                        originLeft = false;
                        originRight = false;
                    }
                }

                if (!keep2) {
                    if (machine.x < box_left && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                        originLeft2 = true;
                        originRight2 = false;
                    } else if (machine.x > box_right && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width || ((machine.y < box_bottom|| machine.y > box_up) && machine.x >= box_left && machine.x <= box_right)) {
                        originLeft2 = false;
                        originRight2 = true;
                    } else {
                        originLeft2 = false;
                        originRight2 = false;
                    }
                }

                if (!keep3) {
                    if (machine.x < gate3 && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                        originLeft3 = true;
                        originRight3 = false;
                    } else if (machine.x > gate3 && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                        originLeft3 = false;
                        originRight3 = true;
                    } else {
                        originLeft3 = false;
                        originRight3 = false;
                    }
                }


                for (int i = 0; i < 4; i++) {
                    int newX = machine.x + directions[i].x;
                    int newY = machine.y + directions[i].y;
                    bool indicator = false;

                    if (newX >= 0 && newX <= hypo_side){
                        if (newY >= newX && newY <= (Height - newX)){
                            DirectionChoice.push_back(tuple(newX, newY));
                            indicator = true;
                        }
                    }
                    else if (newX > hypo_side && newX <= (hypo_side + length)){
                        if (newY >= hypo_side && newY <= (hypo_side + fixed_width)){
                            DirectionChoice.push_back(tuple(newX, newY));
                            indicator = true;
                        }
                    }
                    else if (newX > (hypo_side + length) && newX <= Long){
                        if (newY >= (Long - newX) && newY <= (newX + Height - Long)){
                            DirectionChoice.push_back(tuple(newX, newY));
                            indicator = true;
                        }
                    } 

                    if (!indicator) {
                        double rotatedX = -1*newY + a + b;
                        double rotatedY = newX + b - a;
                        if (rotatedX >= 0 && rotatedX <= hypo_side){
                            if (rotatedY >= rotatedX && rotatedY <= (Height - rotatedX)){
                                DirectionChoice.push_back(tuple(newX, newY));
                            }
                        } else if (rotatedX > hypo_side && rotatedX <= (hypo_side + length)){
                            if (rotatedY >= hypo_side && rotatedY <= (hypo_side + fixed_width)){
                                DirectionChoice.push_back(tuple(newX, newY));
                            }
                        } else if (rotatedX > (hypo_side + length) && rotatedX <= Long){
                            if (rotatedY >= (Long - rotatedX) && rotatedY <= (rotatedX + Height - Long)){
                                DirectionChoice.push_back(tuple(newX, newY));
                            }
                        }
                    }
                    
                }


                if (DirectionChoice.size() <= 0) {
                    std::cerr << "Error: No valid direction choices available." << std::endl;
                    return 1;
                } else {
                    std::uniform_int_distribution<> randomNext(0, DirectionChoice.size()-1);
                    machine = DirectionChoice.at(randomNext(gen));
                }

                if (Find_And_Insert_Node(path, machine, hashSize)) {
                    unique_points += 1;
                }
                walk_count += 1;

                if (machine.x < gate && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                    nextLeft = true;
                    nextRight = false;
                    keep = false;
                } else if (machine.x > gate && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                    nextLeft = false;
                    nextRight = true;
                    keep = false;
                } else {
                    nextLeft = false;
                    nextRight = false;
                    if (machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {keep = true;}
                    else {keep = false;}
                }

                if (nextLeft && originRight || nextRight && originLeft) {
                    gateCrossedCount += 1;
                    nextLeft = false;
                    nextRight = false;
                    originLeft = false;
                    originRight = false;
                    keep = false;
                    finalCrossGate1 = walk_count;
                } 

                //----------------------------------------------------------box 2-------------------------------------------------//
                if (machine.x < box_left && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                    nextLeft2 = true;
                    nextRight2 = false;
                    keep2 = false;
                } else if (machine.x > box_right && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width || ((machine.y < box_bottom || machine.y > box_up) && machine.x >= box_left && machine.x <= box_right)) {
                    nextLeft2 = false;
                    nextRight2 = true;
                    keep2 = false;
                } else {
                    nextLeft2 = false;
                    nextRight2 = false;
                    if (machine.y >= hypo_side && machine.y <= hypo_side + fixed_width && machine.x >= box_left && machine.x <= box_right) {keep2 = true;}
                    else {keep2 = false;}
                }
                if (nextLeft2 && originRight2 || nextRight2 && originLeft2) {
                    gateCrossedCount2 += 1;
                    nextLeft2 = false;
                    nextRight2 = false;
                    originLeft2 = false;
                    originRight2 = false;
                    keep2 = false;
                    finalCrossGate2 = walk_count;
                }

                //-----------------------------------------------------------------------------------------------------------------//
                //----------------------------------------------------------gate 3-------------------------------------------------//
                if (machine.x < gate3 && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                    nextLeft3 = true;
                    nextRight3 = false;
                    keep3 = false;
                } else if (machine.x > gate3 && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                    nextLeft3 = false;
                    nextRight3 = true;
                    keep3 = false;
                } else {
                    nextLeft3 = false;
                    nextRight3 = false;
                    if (machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {keep3 = true;}
                    else {keep3 = false;}
                }

                if (nextLeft3 && originRight3 || nextRight3 && originLeft3) {
                    gateCrossedCount3 += 1;
                    nextLeft3 = false;
                    nextRight3 = false;
                    originLeft3 = false;
                    originRight3 = false;
                    keep3 = false;
                    finalCrossGate3 = walk_count;
                }


            }

    
        }
    }
    return 0;
} 