#include <iostream>
#include <random>
#include <vector>
#include <iomanip>

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

    void setNext(node* nextNode) {
        this->nextNode = nextNode;
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


void printMap(node** hashmap, int size) {
    for (int i = 0; i < size; i++) {
        node* tempoNode = hashmap[i];
        while (tempoNode != nullptr) {
            std::cout << "(" << tempoNode->x << ", " << tempoNode->y << ")" << " -> ";
            tempoNode = tempoNode->nextNode;
        }
        std::cout << std::endl;
    }
}

//--------------------------------------------------------------constant variables-------------------------------------------------------------//
//double hypo_side_list[] = {10.568872793616029, 11.62576007297763, 12.682647352339234, 13.739534631700838, 14.79642191106244, 15.853309190424042, 16.910196469785646, 17.96708374914725, 19.023971028508853, 20.080858307870454, 21.137745587232057, 22.19463286659366, 23.25152014595526, 24.308407425316865, 25.36529470467847, 26.422181984040073, 27.479069263401676, 28.535956542763277, 29.59284382212488, 30.649731101486484, 31.706618380848084, 32.76350566020969, 33.82039293957129, 34.877280218932896, 35.9341674982945, 36.9910547776561, 38.04794205701771, 39.104829336379304, 40.16171661574091, 41.21860389510251, 42.275491174464115, 43.33237845382572, 44.38926573318732, 45.446153012548926, 46.50304029191052, 47.55992757127213, 48.61681485063373, 49.673702129995334, 50.73058940935694, 51.78747668871854, 52.844363968080145, 53.90125124744175, 54.95813852680335, 56.01502580616495, 57.07191308552655, 58.12880036488816, 59.18568764424976, 60.242574923611365, 61.29946220297297, 62.35634948233457, 63.41323676169617, 64.47012404105777, 65.52701132041938, 66.58389859978098, 67.64078587914258, 68.69767315850419, 69.75456043786579, 70.8114477172274, 71.868334996589, 72.9252222759506, 73.9821095553122, 75.03899683467381, 76.09588411403541, 77.152771393397, 78.20965867275861, 79.26654595212021, 80.32343323148181, 81.38032051084342, 82.43720779020502, 83.49409506956663, 84.55098234892823, 85.60786962828983, 86.66475690765144, 87.72164418701304, 88.77853146637464, 89.83541874573625, 90.89230602509785, 91.94919330445946, 93.00608058382105, 94.06296786318265, 95.11985514254425, 96.17674242190586, 97.23362970126746, 98.29051698062906, 99.34740425999067, 100.40429153935227, 101.46117881871388, 102.51806609807548, 103.57495337743708, 104.63184065679869, 105.68872793616029, 106.7456152155219, 107.8025024948835, 108.8593897742451, 109.9162770536067, 110.9731643329683, 112.0300516123299, 113.0869388916915, 114.1438261710531, 115.20071345041471, 116.25760072977631, 117.31448800913792, 118.37137528849952, 119.42826256786113, 120.48514984722273, 121.54203712658433, 122.59892440594594, 123.65581168530754, 124.71269896466914, 125.76958624403075, 126.82647352339234, 127.88336080275394, 128.94024808211555, 129.99713536147715, 131.05402264083875, 132.11090992020036, 133.16779719956196, 134.22468447892356, 135.28157175828517, 136.33845903764677, 137.39534631700838, 138.45223359636998, 139.50912087573158, 140.5660081550932, 141.6228954344548, 142.6797827138164, 143.736669993178, 144.7935572725396, 145.8504445519012, 146.9073318312628, 147.9642191106244, 149.02110638998602, 150.07799366934762, 151.13488094870922, 152.19176822807083, 153.2486555074324, 154.305542786794, 155.3624300661556, 156.41931734551721, 157.47620462487882, 158.53309190424042, 159.58997918360203, 160.64686646296363, 161.70375374232523, 162.76064102168684, 163.81752830104844, 164.87441558041004, 165.93130285977165, 166.98819013913325, 168.04507741849486, 169.10196469785646, 170.15885197721806, 171.21573925657967, 172.27262653594127, 173.32951381530287, 174.38640109466448, 175.44328837402608, 176.50017565338769, 177.5570629327493, 178.6139502121109, 179.6708374914725, 180.7277247708341, 181.7846120501957, 182.8414993295573, 183.8983866089189, 184.95527388828052, 186.0121611676421, 187.0690484470037, 188.1259357263653, 189.1828230057269, 190.2397102850885, 191.2965975644501, 192.3534848438117, 193.41037212317332, 194.46725940253492, 195.52414668189653, 196.58103396125813, 197.63792124061973, 198.69480851998134, 199.75169579934294, 200.80858307870454, 201.86547035806615, 202.92235763742775, 203.97924491678936, 205.03613219615096, 206.09301947551256, 207.14990675487417, 208.20679403423577, 209.26368131359737, 210.32056859295898, 211.37745587232058, 212.43434315168219, 213.4912304310438, 214.5481177104054, 215.605004989767, 216.6618922691286, 217.7187795484902, 218.7756668278518, 219.8325541072134, 220.889441386575, 221.9463286659366, 223.0032159452982, 224.0601032246598, 225.1169905040214, 226.173877783383, 227.2307650627446, 228.2876523421062, 229.34453962146782, 230.40142690082942, 231.45831418019102, 232.51520145955263, 233.57208873891423, 234.62897601827584, 235.68586329763744, 236.74275057699904, 237.79963785636065, 238.85652513572225, 239.91341241508385, 240.97029969444546, 242.02718697380706, 243.08407425316867, 244.14096153253027, 245.19784881189187, 246.25473609125348, 247.31162337061508, 248.36851064997668, 249.4253979293383, 250.4822852086999, 251.5391724880615, 252.5960597674231, 253.65294704678467, 254.70983432614628, 255.76672160550788, 256.8236088848695, 257.8804961642311, 258.9373834435927, 259.9942707229543, 261.0511580023159, 262.1080452816775, 263.1649325610391, 264.2218198404007, 265.2787071197623, 266.3355943991239, 267.3924816784855, 268.4493689578471, 269.50625623720873, 270.56314351657034, 271.62003079593194, 272.67691807529354, 273.73380535465515, 274.79069263401675, 275.84757991337835, 276.90446719273996, 277.96135447210156, 279.01824175146317, 280.07512903082477, 281.1320163101864, 282.188903589548, 283.2457908689096, 284.3026781482712, 285.3595654276328, 286.4164527069944, 287.473339986356, 288.5302272657176, 289.5871145450792, 290.6440018244408, 291.7008891038024, 292.757776383164, 293.8146636625256, 294.8715509418872, 295.9284382212488, 296.98532550061043, 298.04221277997203, 299.09910005933364, 300.15598733869524, 301.21287461805684, 302.26976189741845, 303.32664917678005, 304.38353645614166, 305.4404237355032, 306.4973110148648, 307.5541982942264, 308.611085573588, 309.6679728529496, 310.7248601323112, 311.7817474116728, 312.83863469103443, 313.89552197039603, 314.95240924975764, 316.00929652911924};
//double length_list[] = {13.211090992020036, 14.532200091222037, 15.853309190424042, 17.174418289626047, 18.49552738882805, 19.816636488030053, 21.137745587232057, 22.458854686434062, 23.779963785636067, 25.101072884838068, 26.422181984040073, 27.743291083242077, 29.064400182444075, 30.38550928164608, 31.706618380848084, 33.02772748005009, 34.348836579252094, 35.6699456784541, 36.9910547776561, 38.31216387685811, 39.633272976060105, 40.95438207526211, 42.275491174464115, 43.59660027366612, 44.917709372868124, 46.23881847207013, 47.559927571272134, 48.88103667047413, 50.202145769676136, 51.52325486887814, 52.844363968080145, 54.16547306728215, 55.486582166484155, 56.80769126568616, 58.12880036488815, 59.449909464090155, 60.77101856329216, 62.092127662494164, 63.41323676169617, 64.73434586089817, 66.05545496010018, 67.37656405930218, 68.69767315850419, 70.01878225770619, 71.3398913569082, 72.6610004561102, 73.9821095553122, 75.30321865451421, 76.62432775371622, 77.94543685291822, 79.26654595212021, 80.58765505132222, 81.90876415052422, 83.22987324972623, 84.55098234892823, 85.87209144813023, 87.19320054733224, 88.51430964653424, 89.83541874573625, 91.15652784493825, 92.47763694414026, 93.79874604334226, 95.11985514254427, 96.44096424174626, 97.76207334094826, 99.08318244015027, 100.40429153935227, 101.72540063855428, 103.04650973775628, 104.36761883695829, 105.68872793616029, 107.0098370353623, 108.3309461345643, 109.6520552337663, 110.97316433296831, 112.29427343217031, 113.61538253137232, 114.93649163057432, 116.2576007297763, 117.5787098289783, 118.89981892818031, 120.22092802738231, 121.54203712658432, 122.86314622578632, 124.18425532498833, 125.50536442419033, 126.82647352339234, 128.14758262259434, 129.46869172179635, 130.78980082099835, 132.11090992020036, 133.43201901940236, 134.75312811860437, 136.07423721780637, 137.39534631700838, 138.71645541621038, 140.03756451541238, 141.3586736146144, 142.6797827138164, 144.0008918130184, 145.3220009122204, 146.6431100114224, 147.9642191106244, 149.28532820982642, 150.60643730902842, 151.92754640823043, 153.24865550743243, 154.56976460663444, 155.89087370583644, 157.21198280503845, 158.53309190424042, 159.85420100344243, 161.17531010264443, 162.49641920184644, 163.81752830104844, 165.13863740025045, 166.45974649945245, 167.78085559865445, 169.10196469785646, 170.42307379705846, 171.74418289626047, 173.06529199546247, 174.38640109466448, 175.70751019386648, 177.0286192930685, 178.3497283922705, 179.6708374914725, 180.9919465906745, 182.3130556898765, 183.6341647890785, 184.95527388828052, 186.27638298748252, 187.59749208668453, 188.91860118588653, 190.23971028508853, 191.5608193842905, 192.88192848349252, 194.20303758269452, 195.52414668189653, 196.84525578109853, 198.16636488030053, 199.48747397950254, 200.80858307870454, 202.12969217790655, 203.45080127710855, 204.77191037631056, 206.09301947551256, 207.41412857471457, 208.73523767391657, 210.05634677311858, 211.37745587232058, 212.6985649715226, 214.0196740707246, 215.3407831699266, 216.6618922691286, 217.9830013683306, 219.3041104675326, 220.62521956673461, 221.94632866593662, 223.26743776513862, 224.58854686434063, 225.90965596354263, 227.23076506274464, 228.55187416194664, 229.87298326114865, 231.19409236035065, 232.5152014595526, 233.83631055875463, 235.1574196579566, 236.47852875715864, 237.79963785636062, 239.12074695556265, 240.44185605476463, 241.76296515396666, 243.08407425316864, 244.40518335237067, 245.72629245157265, 247.04740155077468, 248.36851064997666, 249.6896197491787, 251.01072884838067, 252.3318379475827, 253.65294704678467, 254.9740561459867, 256.2951652451887, 257.6162743443907, 258.9373834435927, 260.2584925427947, 261.5796016419967, 262.90071074119874, 264.2218198404007, 265.54292893960275, 266.8640380388047, 268.18514713800676, 269.50625623720873, 270.82736533641076, 272.14847443561274, 273.4695835348148, 274.79069263401675, 276.1118017332187, 277.43291083242076, 278.75401993162274, 280.07512903082477, 281.39623813002675, 282.7173472292288, 284.03845632843075, 285.3595654276328, 286.68067452683476, 288.0017836260368, 289.3228927252388, 290.6440018244408, 291.9651109236428, 293.2862200228448, 294.6073291220468, 295.9284382212488, 297.2495473204508, 298.57065641965283, 299.8917655188548, 301.21287461805684, 302.5339837172588, 303.85509281646085, 305.17620191566283, 306.49731101486486, 307.81842011406684, 309.1395292132689, 310.46063831247085, 311.7817474116729, 313.10285651087486, 314.4239656100769, 315.74507470927887, 317.06618380848084, 318.3872929076829, 319.70840200688485, 321.02951110608683, 322.35062020528886, 323.6717293044909, 324.9928384036929, 326.31394750289485, 327.6350566020969, 328.9561657012989, 330.2772748005009, 331.59838389970287, 332.9194929989049, 334.24060209810693, 335.5617111973089, 336.8828202965109, 338.2039293957129, 339.52503849491495, 340.8461475941169, 342.1672566933189, 343.48836579252094, 344.80947489172297, 346.13058399092495, 347.4516930901269, 348.77280218932896, 350.093911288531, 351.41502038773297, 352.73612948693494, 354.057238586137, 355.378347685339, 356.699456784541, 358.02056588374296, 359.341674982945, 360.662784082147, 361.983893181349, 363.305002280551, 364.626111379753, 365.94722047895505, 367.268329578157, 368.589438677359, 369.91054777656103, 371.23165687576306, 372.55276597496504, 373.873875074167, 375.19498417336905, 376.5160932725711, 377.83720237177306, 379.15831147097504, 380.47942057017707, 381.800529669379, 383.121638768581, 384.442747867783, 385.76385696698503, 387.084966066187, 388.40607516538904, 389.727184264591, 391.04829336379305, 392.369402462995, 393.69051156219706, 395.01162066139904};
double width_list[] = {10.040429153935227, 10.304650973775628, 10.568872793616029, 10.83309461345643, 11.09731643329683, 11.361538253137232, 11.62576007297763, 11.889981892818032, 12.154203712658433, 12.418425532498834, 12.682647352339234, 12.946869172179635, 13.211090992020036, 13.475312811860437, 13.739534631700838, 14.003756451541237, 14.267978271381638, 14.53220009122204, 14.79642191106244, 15.060643730902841, 15.324865550743242, 15.589087370583643, 15.853309190424042, 16.117531010264443, 16.381752830104844, 16.645974649945245, 16.910196469785646, 17.174418289626047, 17.438640109466448, 17.70286192930685, 17.96708374914725, 18.23130556898765, 18.49552738882805, 18.759749208668453, 19.023971028508853, 19.28819284834925, 19.552414668189652, 19.816636488030053, 20.080858307870454, 20.345080127710855, 20.609301947551256, 20.873523767391656, 21.137745587232057, 21.40196740707246, 21.66618922691286, 21.93041104675326, 22.19463286659366, 22.458854686434062, 22.723076506274463, 22.987298326114864, 23.25152014595526, 23.515741965795662, 23.779963785636063, 24.044185605476464, 24.308407425316865, 24.572629245157266, 24.836851064997667, 25.101072884838068, 25.36529470467847, 25.62951652451887, 25.89373834435927, 26.15796016419967, 26.422181984040073, 26.686403803880474, 26.950625623720875, 27.214847443561275, 27.479069263401676, 27.743291083242074, 28.007512903082475, 28.271734722922876, 28.535956542763277, 28.800178362603678, 29.06440018244408, 29.32862200228448, 29.59284382212488, 29.85706564196528, 30.121287461805682, 30.385509281646083, 30.649731101486484, 30.913952921326885, 31.178174741167286, 31.442396561007687, 31.706618380848084, 31.970840200688485, 32.235062020528886, 32.49928384036929, 32.76350566020969, 33.02772748005009, 33.29194929989049, 33.55617111973089, 33.82039293957129, 34.08461475941169, 34.348836579252094, 34.613058399092495, 34.877280218932896, 35.1415020387733, 35.4057238586137, 35.6699456784541, 35.9341674982945, 36.1983893181349, 36.4626111379753, 36.7268329578157, 36.9910547776561, 37.255276597496504, 37.519498417336905, 37.783720237177306, 38.04794205701771, 38.3121638768581, 38.5763856966985, 38.8406075165389, 39.104829336379304, 39.369051156219705, 39.633272976060105, 39.897494795900506, 40.16171661574091, 40.42593843558131, 40.69016025542171, 40.95438207526211, 41.21860389510251, 41.48282571494291, 41.74704753478331, 42.011269354623714, 42.275491174464115, 42.539712994304516, 42.80393481414492, 43.06815663398532, 43.33237845382572, 43.59660027366612, 43.86082209350652, 44.12504391334692, 44.38926573318732, 44.65348755302772, 44.917709372868124, 45.181931192708525, 45.446153012548926, 45.71037483238933, 45.97459665222973, 46.23881847207013, 46.50304029191052, 46.767262111750924, 47.031483931591325, 47.295705751431726, 47.55992757127213, 47.82414939111253, 48.08837121095293, 48.35259303079333, 48.61681485063373, 48.88103667047413, 49.14525849031453, 49.40948031015493, 49.673702129995334, 49.937923949835735, 50.202145769676136, 50.46636758951654, 50.73058940935694, 50.99481122919734, 51.25903304903774, 51.52325486887814, 51.78747668871854, 52.05169850855894, 52.31592032839934, 52.580142148239744, 52.844363968080145, 53.108585787920546, 53.37280760776095, 53.63702942760135, 53.90125124744175, 54.16547306728215, 54.42969488712255, 54.69391670696295, 54.95813852680335, 55.22236034664375, 55.48658216648415, 55.75080398632455, 56.01502580616495, 56.27924762600535, 56.54346944584575, 56.80769126568615, 57.07191308552655, 57.336134905366954, 57.600356725207355, 57.864578545047756, 58.12880036488816, 58.39302218472856, 58.65724400456896, 58.92146582440936, 59.18568764424976, 59.44990946409016, 59.71413128393056, 59.978353103770964, 60.242574923611365, 60.506796743451766, 60.77101856329217, 61.03524038313257, 61.29946220297297, 61.56368402281337, 61.82790584265377, 62.09212766249417, 62.35634948233457, 62.62057130217497, 62.884793122015374, 63.149014941855775, 63.41323676169617, 63.67745858153657, 63.94168040137697, 64.20590222121737, 64.47012404105777, 64.73434586089817, 64.99856768073857, 65.26278950057898, 65.52701132041938, 65.79123314025978, 66.05545496010018, 66.31967677994058, 66.58389859978098, 66.84812041962138, 67.11234223946178, 67.37656405930218, 67.64078587914258, 67.90500769898298, 68.16922951882339, 68.43345133866379, 68.69767315850419, 68.96189497834459, 69.22611679818499, 69.49033861802539, 69.75456043786579, 70.01878225770619, 70.2830040775466, 70.547225897387, 70.8114477172274, 71.0756695370678, 71.3398913569082, 71.6041131767486, 71.868334996589, 72.1325568164294, 72.3967786362698, 72.6610004561102, 72.9252222759506, 73.189444095791, 73.4536659156314, 73.7178877354718, 73.9821095553122, 74.24633137515261, 74.51055319499301, 74.77477501483341, 75.03899683467381, 75.30321865451421, 75.56744047435461, 75.83166229419501, 76.09588411403541, 76.3601059338758, 76.6243277537162, 76.8885495735566, 77.152771393397, 77.4169932132374, 77.6812150330778, 77.9454368529182, 78.20965867275861, 78.47388049259901, 78.73810231243941, 79.00232413227981};
//Need change ↓
double point_list[] = {15441, 16010, 16625, 17249, 17881, 18612, 19396, 19880, 20684, 21360, 22004, 22696, 23396, 24104, 24930, 25809, 26351, 27209, 27961, 28722, 29489, 30225, 31049, 31970, 32864, 33542, 34496, 35324, 36160, 37004, 37816, 38716, 39732, 40531, 41457, 42505, 43409, 44322, 45241, 46129, 47066, 48216, 49090, 50092, 51236, 52216, 53382, 54160, 55164, 56360, 57609, 58373, 59641, 60689, 61745, 63006, 63841, 64921, 66212, 67556, 68376, 69740, 70864, 71956, 73096, 74244, 75400, 76786, 78225, 79103, 80521, 81761, 82930, 84145, 85329, 86601, 88082, 89576, 90550, 92064, 93340, 94624, 95916, 97176, 98524, 100100, 101650, 102721, 104329, 105681, 107042, 108409, 109745, 111130, 113120, 114162, 115612, 117316, 118744, 120470, 121624, 123036, 124792, 126601, 127701, 129529, 131025, 132529, 134350, 135521, 137049, 138900, 140804, 141960, 143884, 145456, 146996, 148952, 150180, 151784, 153730, 155729, 156943, 158961, 160609, 162226, 163889, 165521, 167241, 169636, 171336, 172646, 174720, 176444, 178176, 179916, 181624, 183420, 185929, 187666, 189073, 191241, 193041, 194850, 196665, 198449, 200321, 202944, 204756, 206220, 208484, 210360, 212244, 214136, 215996, 218312, 220681, 222117, 224505, 226449, 228401, 230782, 232289, 234265, 236676, 239140, 240632, 243116, 245136, 247164, 249640, 251204, 253256, 256209, 258321, 259871, 262449, 264545, 266610, 269220, 270841, 272969, 276036, 278184, 279830, 282464, 284636, 286816, 289004, 291160, 293404, 296585, 298809, 300513, 303241, 305489, 307746, 310009, 312241, 314561, 317856, 320116, 321916, 324740, 327064, 329396, 331736, 334044, 336920, 339849, 342186, 344569, 346961, 349361, 352302, 354185, 356569, 360080, 362564, 364976, 367436, 369904, 372380, 375416, 377316, 379816, 383441, 386001, 387887, 391025, 393569, 396082, 399252, 401209, 403785, 407524, 410120, 412102, 415336, 417916, 420544, 423810, 425784, 428476, 432329, 435001, 437041, 440329, 443025, 445730, 449090, 451121, 453889, 457856, 460564, 462700, 466084, 468856, 471636, 474424, 477180, 480657};

const int sample_size = 1500; //for random walk
const tuple directions[4] {tuple(-1, 0), tuple(1, 0), tuple(0, -1), tuple(0, 1)};

const double fixed_width = 40.0;

/*
int valueList[sample_size];

int gateValueListLeft[sample_size];
int gateValueListRight[sample_size];

int gateValueListLeft2[sample_size];
int gateValueListRight2[sample_size];

int gateValueListLeft3[sample_size];
int gateValueListRight3[sample_size];

int gate1StopLeft[sample_size];
int gate1StopRight[sample_size];
int gate2StopLeft[sample_size];
int gate2StopRight[sample_size];
int gate3StopLeft[sample_size];
int gate3StopRight[sample_size];

*/
//--------------------------------------------------------------main function-----------------------------------------------------------------//
int main() {
    
    std::random_device rd;
    std::mt19937 gen(rd());


    for (int element = 0; element < 300-38; element++) {

        //if (element <= 228) {continue;}

        /*
        std::fill(valueList, valueList + sample_size, 0);
        std::fill(gateValueListLeft, gateValueListLeft + sample_size, 0);
        std::fill(gateValueListRight, gateValueListRight + sample_size, 0);
        std::fill(gateValueListLeft2, gateValueListLeft2 + sample_size, 0);
        std::fill(gateValueListRight2, gateValueListRight2 + sample_size, 0);
        std::fill(gateValueListLeft3, gateValueListLeft3 + sample_size, 0);
        std::fill(gateValueListRight3, gateValueListRight3 + sample_size, 0);
        std::fill(gate1StopLeft, gate1StopLeft + sample_size, 0);
        std::fill(gate1StopRight, gate1StopRight + sample_size, 0);
        std::fill(gate2StopLeft, gate2StopLeft + sample_size, 0);
        std::fill(gate2StopRight, gate2StopRight + sample_size, 0);
        std::fill(gate3StopLeft, gate3StopLeft + sample_size, 0);
        std::fill(gate3StopRight, gate3StopRight + sample_size, 0);
        */
        std::vector<int> valueList;
        std::vector<int> gateValueListLeft;
        std::vector<int> gateValueListRight;
        std::vector<int> gateValueListLeft2;
        std::vector<int> gateValueListRight2;
        std::vector<int> gateValueListLeft3;
        std::vector<int> gateValueListRight3;

        std::vector<int> gateValueListLeft_vertical;
        std::vector<int> gateValueListRight_vertical;
        std::vector<int> gateValueListLeft2_vertical;
        std::vector<int> gateValueListRight2_vertical;
        std::vector<int> gateValueListLeft3_vertical;
        std::vector<int> gateValueListRight3_vertical;

        std::vector<int> gate1StopLeft;
        std::vector<int> gate1StopRight;
        std::vector<int> gate2StopLeft;
        std::vector<int> gate2StopRight;
        std::vector<int> gate3StopLeft;
        std::vector<int> gate3StopRight;

        std::vector<int> gate1StopLeft_vertical;
        std::vector<int> gate1StopRight_vertical;
        std::vector<int> gate2StopLeft_vertical;
        std::vector<int> gate2StopRight_vertical;
        std::vector<int> gate3StopLeft_vertical;
        std::vector<int> gate3StopRight_vertical;
        


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

        const double gate_vertical = vertical_top - (0.25 * fixed_width + hypo_side);
        const double gate2_vertical = vertical_top - (0.5 * fixed_width + hypo_side);
        const double gate3_vertical = vertical_top - (0.75 * fixed_width + hypo_side); //for random walk

        const double a = hypo_side + length/2;
        const double b = hypo_side + fixed_width/2;

        const double vertical_left = a - fixed_width/2;
        const double vertical_right = a + fixed_width/2;


        const int hashSize = 2 * totalPoints;

        std::uniform_int_distribution<> randomX(0, Long+2);
        std::uniform_int_distribution<> randomY(vertical_bottom - 2, vertical_top + 2);

        for (int overall_count = 0; overall_count < sample_size; overall_count++) {
            //------------------------------------------initialize-------------------------------------------//
            tuple machine(0, 0);
            int gateCrossedCount = 0;
            int gateCrossedCount2 = 0;
            int gateCrossedCount3 = 0;

            int gateCrossedCount_vertical = 0;
            int gateCrossedCount2_vertical = 0;
            int gateCrossedCount3_vertical = 0;

            while (true) {
                int machineX = randomX(gen);
                int machineY = randomY(gen);

                int machineX_new = -1*machineY + a + b;
                int machineY_new = machineX + b - a;

                bool indicator = false;

                if (machineX == gate || machineX == gate2 || machineX == gate3 || machineX_new == gate || machineX_new == gate2 || machineX_new == gate3) {
                    continue; // Skip if the machine is at the gate
                }

                if (machineX <= hypo_side){
                    if (machineY >= machineX && machineY <= Height - machineX){
                        machine = tuple(machineX, machineY);
                        indicator = true;
                        break;
                    }
                } else if (machineX > hypo_side && machineX <= hypo_side + length){
                    if (machineY >= hypo_side && machineY <= hypo_side + fixed_width){
                        machine = tuple(machineX, machineY);
                        indicator = true;
                        break;
                    }
                } else if (machineX > hypo_side + length && machineX <= Long){
                    if (machineY >= Long-machineX && machineY <= machineX + Height - Long){
                        machine = tuple(machineX, machineY);
                        indicator = true;
                        break;
                    }
                } 

                if (!indicator) {
                    double rotatedX = -1*machineY + a + b;
                    double rotatedY = machineX + b - a;
                    if (rotatedX >= 0 && rotatedX <= hypo_side){
                        if (rotatedY >= rotatedX && rotatedY <= Height - rotatedX){
                            machine = tuple(machineX, machineY);
                            break;
                        }
                    } else if (rotatedX > hypo_side && rotatedX <= (hypo_side + length)){
                        if (rotatedY >= hypo_side && rotatedY <= (hypo_side + fixed_width)){
                            machine = tuple(machineX, machineY);
                            break;
                        }
                    } else if (rotatedX > (hypo_side + length) && rotatedX <= Long){
                        if (rotatedY >= (Long - rotatedX) && rotatedY <= (rotatedX + Height - Long)){
                            machine = tuple(machineX, machineY);
                            break;
                        }
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

            bool nextLeft_vertical, nextRight_vertical, originLeft_vertical, originRight_vertical;
            bool keep_vertical = false;

            bool nextLeft2_vertical, nextRight2_vertical, originLeft2_vertical, originRight2_vertical;
            bool keep2_vertical = false;

            bool nextLeft3_vertical, nextRight3_vertical, originLeft3_vertical, originRight3_vertical;
            bool keep3_vertical = false;

            int uniquePointGate1Left = 0; 
            int uniquePointGate1Right = 0;
            int uniquePointGate2Left = 0;
            int uniquePointGate2Right = 0;
            int uniquePointGate3Left = 0;
            int uniquePointGate3Right = 0;

            int uniquePointGate1Left_vertical = 0;
            int uniquePointGate1Right_vertical = 0;
            int uniquePointGate2Left_vertical = 0;
            int uniquePointGate2Right_vertical = 0;
            int uniquePointGate3Left_vertical = 0;
            int uniquePointGate3Right_vertical = 0;
            
            while (unique_points < totalPoints_half) {
                std::vector<tuple> DirectionChoice;

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
                    if (machine.x < gate2 && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                        originLeft2 = true;
                        originRight2 = false;
                    } else if (machine.x > gate2 && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
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

                if (!keep_vertical) {
                    if (machine.y < gate_vertical && machine.x >= vertical_left && machine.x <= vertical_right) {
                        originLeft_vertical = true;
                        originRight_vertical = false;
                    } else if (machine.y > gate_vertical && machine.x >= vertical_left && machine.x <= vertical_right) {
                        originLeft_vertical = false;
                        originRight_vertical = true;
                    } else {
                        originLeft_vertical = false;
                        originRight_vertical = false;
                    }
                }

                if (!keep2_vertical) {
                    if (machine.y < gate2_vertical && machine.x >= vertical_left && machine.x <= vertical_right) {
                        originLeft2_vertical = true;
                        originRight2_vertical = false;
                    } else if (machine.y > gate2_vertical && machine.x >= vertical_left && machine.x <= vertical_right) {
                        originLeft2_vertical = false;
                        originRight2_vertical = true;
                    } else {
                        originLeft2_vertical = false;
                        originRight2_vertical = false;
                    }
                }

                if (!keep3_vertical) {
                    if (machine.y < gate3_vertical && machine.x >= vertical_left && machine.x <= vertical_right) {
                        originLeft3_vertical = true;
                        originRight3_vertical = false;
                    } else if (machine.y > gate3_vertical && machine.x >= vertical_left && machine.x <= vertical_right) {
                        originLeft3_vertical = false;
                        originRight3_vertical = true;
                    } else {
                        originLeft3_vertical = false;
                        originRight3_vertical = false;
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

                if (nextLeft && originRight) {
                    gateCrossedCount += 1;
                    nextLeft = false;
                    nextRight = false;
                    originLeft = false;
                    originRight = false;
                    keep = false;
                    uniquePointGate1Left = walk_count;
                } 
                else if (nextRight && originLeft) {
                    gateCrossedCount += 1;
                    nextLeft = false;
                    nextRight = false;
                    originLeft = false;
                    originRight = false;
                    keep = false;
                    uniquePointGate1Right = walk_count;
                }

                //----------------------------------------------------------gate 2-------------------------------------------------//
                if (machine.x < gate2 && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                    nextLeft2 = true;
                    nextRight2 = false;
                    keep2 = false;
                } else if (machine.x > gate2 && machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {
                    nextLeft2 = false;
                    nextRight2 = true;
                    keep2 = false;
                } else {
                    nextLeft2 = false;
                    nextRight2 = false;
                    if (machine.y >= hypo_side && machine.y <= hypo_side + fixed_width) {keep2 = true;}
                    else {keep2 = false;}
                }
                if (nextLeft2 && originRight2) {
                    gateCrossedCount2 += 1;
                    nextLeft2 = false;
                    nextRight2 = false;
                    originLeft2 = false;
                    originRight2 = false;
                    keep2 = false;
                    uniquePointGate2Left = walk_count;
                }
                else if (nextRight2 && originLeft2) {
                    gateCrossedCount2 += 1;
                    nextLeft2 = false;
                    nextRight2 = false;
                    originLeft2 = false;
                    originRight2 = false;
                    keep2 = false;
                    uniquePointGate2Right = walk_count;
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

                if (nextLeft3 && originRight3) {
                    gateCrossedCount3 += 1;
                    nextLeft3 = false;
                    nextRight3 = false;
                    originLeft3 = false;
                    originRight3 = false;
                    keep3 = false;
                    uniquePointGate3Left = walk_count;
                }
                else if (nextRight3 && originLeft3) {
                    gateCrossedCount3 += 1;
                    nextLeft3 = false;
                    nextRight3 = false;
                    originLeft3 = false;
                    originRight3 = false;
                    keep3 = false;
                    uniquePointGate3Right = walk_count;
                }
                //-----------------------------------------------------------------------------------------------------------------//

                
                //std::cout << gateCrossedCount2 << std::endl;

                //----------------------------------------------------------vertical gate-------------------------------------------------//
                if (machine.y < gate_vertical && machine.x >= vertical_left && machine.x <= vertical_right) {
                    nextLeft_vertical = true;
                    nextRight_vertical = false;
                    keep_vertical = false;  
                } else if (machine.y > gate_vertical && machine.x >= vertical_left && machine.x <= vertical_right) {
                    nextLeft_vertical = false;
                    nextRight_vertical = true;
                    keep_vertical = false;
                } else {
                    nextLeft_vertical = false;
                    nextRight_vertical = false;
                    if (machine.x >= vertical_left && machine.x <= vertical_right) {keep_vertical = true;}
                    else {keep_vertical = false;}
                }

                if (nextLeft_vertical && originRight_vertical) {
                    gateCrossedCount_vertical += 1;
                    nextLeft_vertical = false;
                    nextRight_vertical = false;
                    originLeft_vertical = false;
                    originRight_vertical = false;
                    keep_vertical = false;
                    uniquePointGate1Left_vertical = walk_count;
                } 
                else if (nextRight_vertical && originLeft_vertical) {
                    gateCrossedCount_vertical += 1;
                    nextLeft_vertical = false;
                    nextRight_vertical = false;
                    originLeft_vertical = false;
                    originRight_vertical = false;
                    keep_vertical = false;
                    uniquePointGate1Right_vertical = walk_count;
                }

                //----------------------------------------------------------vertical gate 2-------------------------------------------------//
                if (machine.y < gate2_vertical && machine.x >= vertical_left && machine.x <= vertical_right) {
                    nextLeft2_vertical = true;
                    nextRight2_vertical = false;
                    keep2_vertical = false;
                } else if (machine.y > gate2_vertical && machine.x >= vertical_left && machine.x <= vertical_right) {
                    nextLeft2_vertical = false;
                    nextRight2_vertical = true;
                    keep2_vertical = false;
                } else {
                    nextLeft2_vertical = false;
                    nextRight2_vertical = false;
                    if (machine.x >= vertical_left && machine.x <= vertical_right) {keep2_vertical = true;}
                    else {keep2_vertical = false;}
                }

                if (nextLeft2_vertical && originRight2_vertical) {
                    gateCrossedCount2_vertical += 1;
                    nextLeft2_vertical = false;
                    nextRight2_vertical = false;
                    originLeft2_vertical = false;
                    originRight2_vertical = false;
                    keep2_vertical = false;
                    uniquePointGate2Left_vertical = walk_count;
                }
                else if (nextRight2_vertical && originLeft2_vertical) {
                    gateCrossedCount2_vertical += 1;
                    nextLeft2_vertical = false;
                    nextRight2_vertical = false;
                    originLeft2_vertical = false;
                    originRight2_vertical = false;
                    keep2_vertical = false;
                    uniquePointGate2Right_vertical = walk_count;
                }

                //-----------------------------------------------------------------------------------------------------------------//
                //----------------------------------------------------------vertical gate 3-------------------------------------------------//
                if (machine.y < gate3_vertical && machine.x >= vertical_left && machine.x <= vertical_right) {
                    nextLeft3_vertical = true;
                    nextRight3_vertical = false;
                    keep3_vertical = false;
                } else if (machine.y > gate3_vertical && machine.x >= vertical_left && machine.x <= vertical_right) {
                    nextLeft3_vertical = false;
                    nextRight3_vertical = true;
                    keep3_vertical = false;
                } else {
                    nextLeft3_vertical = false;
                    nextRight3_vertical = false;
                    if (machine.x >= vertical_left && machine.x <= vertical_right) {keep3_vertical = true;}
                    else {keep3_vertical = false;}
                }

                if (nextLeft3_vertical && originRight3_vertical) {
                    gateCrossedCount3_vertical += 1;
                    nextLeft3_vertical = false;
                    nextRight3_vertical = false;
                    originLeft3_vertical = false;
                    originRight3_vertical = false;
                    keep3_vertical = false;
                    uniquePointGate3Left_vertical = walk_count;
                }
                else if (nextRight3_vertical && originLeft3_vertical) {
                    gateCrossedCount3_vertical += 1;
                    nextLeft3_vertical = false;
                    nextRight3_vertical = false;
                    originLeft3_vertical = false;
                    originRight3_vertical = false;
                    keep3_vertical = false;
                    uniquePointGate3Right_vertical = walk_count;
                }

            }
            //--------------------------------------------------------------------------------------------------------



            //-----------------------------------------------------------count the partial points-------------------------------------------------//
            int leftTrapozoidPoints = 0;
            int rightTrapozoidPoints = 0;

            int leftTrapozoidPoints_vertical = 0;
            int rightTrapozoidPoints_vertical = 0;

            //int middleRectanglePoints = 0;
            /*
            int GateLeftPoints = 0;
            int GateRightPoints = 0;
            int GateLeftPoints_vertical = 0;
            int GateRightPoints_vertical = 0;
            */

            for (int i = 0; i < hashSize; i++) {
                node* tempoNode = path[i];
                while (tempoNode != nullptr) {
                    bool indicator = false;
                    if (tempoNode->x <= hypo_side){
                        if (tempoNode->y >= tempoNode->x and tempoNode->y <= Height - tempoNode->x){
                            //leftTrapozoidPoints += 1;
                            leftTrapozoidPoints += 1;
                            //GateLeftPoints += 1;
                            indicator = true;
                        } 
                    }  else if (tempoNode->x > hypo_side + length and tempoNode->x <= Long){
                        if (tempoNode->y >= (Long - tempoNode->x) and tempoNode->y <= (tempoNode->x + Height - Long)){
                            //rightTrapozoidPoints += 1;
                            //GateRightPoints += 1;
                            rightTrapozoidPoints += 1;
                            indicator = true;
                        }
                    }

                    if (!indicator) {
                        double rotatedX = -1*tempoNode->y + a + b;
                        double rotatedY = tempoNode->x + b - a;
                        if (rotatedX >= 0 && rotatedX <= hypo_side){
                            if (rotatedY >= rotatedX && rotatedY <= Height - rotatedX){
                                leftTrapozoidPoints_vertical += 1;
                                //GateLeftPoints += 1;
                            }
                        } else if (rotatedX > (hypo_side + length) && rotatedX <= Long){
                            if (rotatedY >= (Long - rotatedX) && rotatedY <= (rotatedX + Height - Long)){
                                rightTrapozoidPoints_vertical += 1;
                                //GateRightPoints += 1;
                            }
                        }
                    }

                    tempoNode = tempoNode->nextNode;
                }
            }
            //--------------------------------------------------------------------------------------------------------


            std::cout << "\rSample " << overall_count << ": Total walk time: " << walk_count << std::flush;


            valueList.push_back(walk_count);

            if (leftTrapozoidPoints > rightTrapozoidPoints) {
                gateValueListLeft.push_back(gateCrossedCount);
                gateValueListLeft2.push_back(gateCrossedCount2);
                gateValueListLeft3.push_back(gateCrossedCount3);

                if (uniquePointGate1Right != 0) {
                    gate1StopLeft.push_back(uniquePointGate1Right);   //unique point is actually the total walk time
                }
                if (uniquePointGate2Right != 0) {
                    gate2StopLeft.push_back(uniquePointGate2Right); //StopLeft means start from left trapozoid and end at gate right
                }
                if (uniquePointGate3Right != 0) {
                    gate3StopLeft.push_back (uniquePointGate3Right);
                }

            } else if (leftTrapozoidPoints < rightTrapozoidPoints) {
                gateValueListRight.push_back(gateCrossedCount);
                gateValueListRight2.push_back(gateCrossedCount2);
                gateValueListRight3.push_back(gateCrossedCount3);

                if (uniquePointGate1Left != 0) {
                    gate1StopRight.push_back(uniquePointGate1Left);   //unique point is actually the total walk time
                }
                if (uniquePointGate2Left != 0) {
                    gate2StopRight.push_back(uniquePointGate2Left); //StopRight means start from right trapozoid and end at gate left
                }
                if (uniquePointGate3Left != 0) {
                    gate3StopRight.push_back(uniquePointGate3Left);
                }
            }


            if (leftTrapozoidPoints_vertical > rightTrapozoidPoints_vertical) {
                gateValueListLeft_vertical.push_back(gateCrossedCount_vertical);
                gateValueListLeft2_vertical.push_back(gateCrossedCount2_vertical);
                gateValueListLeft3_vertical.push_back(gateCrossedCount3_vertical);

                if (uniquePointGate1Right_vertical != 0) {
                    gate1StopLeft_vertical.push_back(uniquePointGate1Right_vertical);   //unique point is actually the total walk time
                }
                if (uniquePointGate2Right_vertical != 0) {
                    gate2StopLeft_vertical.push_back(uniquePointGate2Right_vertical); //StopLeft means start from left trapozoid and end at gate right
                }
                if (uniquePointGate3Right_vertical != 0) {
                    gate3StopLeft_vertical.push_back (uniquePointGate3Right_vertical);
                }

            } else if (leftTrapozoidPoints_vertical < rightTrapozoidPoints_vertical) {
                gateValueListRight_vertical.push_back(gateCrossedCount_vertical);
                gateValueListRight2_vertical.push_back(gateCrossedCount2_vertical);
                gateValueListRight3_vertical.push_back(gateCrossedCount3_vertical);

                if (uniquePointGate1Left_vertical != 0) {
                    gate1StopRight_vertical.push_back(uniquePointGate1Left_vertical);   //unique point is actually the total walk time
                }
                if (uniquePointGate2Left_vertical != 0) {
                    gate2StopRight_vertical.push_back(uniquePointGate2Left_vertical); //StopRight means start from right trapozoid and end at gate left
                }
                if (uniquePointGate3Left_vertical != 0) {
                    gate3StopRight_vertical.push_back(uniquePointGate3Left_vertical);
                }
            }

            //overallGateList[overall_count] = gateCrossedCount;

            //To clear the memory
            for (int i = 0; i < hashSize; i++) {
                node* tempoNode = path[i];
                while (tempoNode != nullptr) {
                    node* temp = tempoNode;
                    tempoNode = tempoNode->nextNode;
                    delete temp;
                    temp = nullptr;
                }
            }
        }


        // ----------------------------------------------------------Overall output-------------------------------------------------//
        double sumMean = 0;
        //double sumGateMean = 0;
        double sumGateLeftMean = 0;
        double sumGateLeftMean2 = 0;
        double sumGateLeftMean3 = 0;

        double sumGateRightMean = 0;
        double sumGateRightMean2 = 0;
        double sumGateRightMean3 = 0;

        double sumGate1StopLeft = 0;
        double sumGate1StopRight = 0;
        double sumGate2StopLeft = 0;
        double sumGate2StopRight = 0;
        double sumGate3StopLeft = 0;
        double sumGate3StopRight = 0;



        double sumGateLeftMean_vertical = 0;
        double sumGateLeftMean2_vertical = 0;
        double sumGateLeftMean3_vertical = 0;

        double sumGateRightMean_vertical = 0;
        double sumGateRightMean2_vertical = 0;
        double sumGateRightMean3_vertical = 0;

        double sumGate1StopLeft_vertical = 0;
        double sumGate1StopRight_vertical = 0;
        double sumGate2StopLeft_vertical = 0;
        double sumGate2StopRight_vertical = 0;
        double sumGate3StopLeft_vertical = 0;
        double sumGate3StopRight_vertical = 0;

        for (int i = 0; i < sample_size; i++) {
            sumMean += valueList[i];
            //sumGateMean += overallGateList[i];
        }
        for (int i = 0; i < gateValueListLeft.size(); i++) {
            sumGateLeftMean += gateValueListLeft[i];
            sumGateLeftMean2 += gateValueListLeft2[i];
            sumGateLeftMean3 += gateValueListLeft3[i];
        }
        for (int i = 0; i < gateValueListRight.size(); i++) {
            sumGateRightMean += gateValueListRight[i];
            sumGateRightMean2 += gateValueListRight2[i];
            sumGateRightMean3 += gateValueListRight3[i];
        }
        for (int i = 0; i < gate1StopLeft.size(); i++) {
            sumGate1StopLeft += gate1StopLeft[i];
        }
        for (int i = 0; i < gate1StopRight.size(); i++) {
            sumGate1StopRight += gate1StopRight[i];
        }
        for (int i = 0; i < gate2StopLeft.size(); i++) {
            sumGate2StopLeft += gate2StopLeft[i];
        }
        for (int i = 0; i < gate2StopRight.size(); i++) {
            sumGate2StopRight += gate2StopRight[i];
        }
        for (int i = 0; i < gate3StopLeft.size(); i++) {
            sumGate3StopLeft += gate3StopLeft[i];
        }
        for (int i = 0; i < gate3StopRight.size(); i++) {
            sumGate3StopRight += gate3StopRight[i];
        }


        for (int i = 0; i < gateValueListLeft_vertical.size(); i++) {
            sumGateLeftMean_vertical += gateValueListLeft_vertical[i];
            sumGateLeftMean2_vertical += gateValueListLeft2_vertical[i];
            sumGateLeftMean3_vertical += gateValueListLeft3_vertical[i];
        }
        for (int i = 0; i < gateValueListRight_vertical.size(); i++) {
            sumGateRightMean_vertical += gateValueListRight_vertical[i];
            sumGateRightMean2_vertical += gateValueListRight2_vertical[i];
            sumGateRightMean3_vertical += gateValueListRight3_vertical[i];
        }
        for (int i = 0; i < gate1StopLeft_vertical.size(); i++) {
            sumGate1StopLeft_vertical += gate1StopLeft_vertical[i];
        }
        for (int i = 0; i < gate1StopRight_vertical.size(); i++) {
            sumGate1StopRight_vertical += gate1StopRight_vertical[i];
        }
        for (int i = 0; i < gate2StopLeft_vertical.size(); i++) {
            sumGate2StopLeft_vertical += gate2StopLeft_vertical[i];
        }
        for (int i = 0; i < gate2StopRight_vertical.size(); i++) {
            sumGate2StopRight_vertical += gate2StopRight_vertical[i];
        }
        for (int i = 0; i < gate3StopLeft_vertical.size(); i++) {
            sumGate3StopLeft_vertical += gate3StopLeft_vertical[i];
        }
        for (int i = 0; i < gate3StopRight_vertical.size(); i++) {
            sumGate3StopRight_vertical += gate3StopRight_vertical[i];
        }
        

        //LTOF: Last time out of ...
        // i-th time, total point, Average walk time, gate1 count (Left), average LTOF gate1 (left), gate2 count (Left), average LTOF gate2 (left), gate3 count (Left), average LTOF gate3 (left), gate1 count (Right), average LTOF gate1 (right), gate2 count (Right), average LTOF gate2 (right), gate3 count (Right), average LTOF gate3 (right), gate1 count (Left_vertical (upper)), average LTOF gate1 (Left_vertical (upper)), gate2 count (Left_vertical (upper)), average LTOF gate2 (Left_vertical (upper)), gate3 count (Left_vertical (upper)), average LTOF gate3 (Left_vertical (upper)), gate1 count (Right_vertical(bottom)), average LTOF gate1 (Right_vertical(bottom)), gate2 count (Right_vertical(bottom)), average LTOF gate2 (Right_vertical(bottom)), gate3 count (Right_vertical(bottom)), average LTOF gate3 (Right_vertical(bottom))
        std::cout << "\r" << element << ',' << point_list[element] << ',' << std::fixed << std::setprecision(6) << sumMean / sample_size << ',' <<sumGateLeftMean / gateValueListLeft.size() << ',' << sumGate1StopLeft / gate1StopLeft.size() << ',' << sumGateLeftMean2 / gateValueListLeft.size() << ',' << sumGate2StopLeft / gate2StopLeft.size() << ',' << sumGateLeftMean3 / gateValueListLeft.size() << ',' << sumGate3StopLeft / gate3StopLeft.size() << ',' << sumGateRightMean / gateValueListRight.size() << ',' << sumGate1StopRight / gate1StopRight.size() << ',' << sumGateRightMean2 / gateValueListRight.size() << ',' << sumGate2StopRight / gate2StopRight.size() << ','  << sumGateRightMean3 / gateValueListRight.size() << ',' << sumGate3StopRight / gate3StopRight.size() << ',' << sumGateLeftMean_vertical / gateValueListLeft_vertical.size() << ',' << sumGate1StopLeft_vertical / gate1StopLeft_vertical.size() << ',' << sumGateLeftMean2_vertical / gateValueListLeft2_vertical.size() << ',' << sumGate2StopLeft_vertical / gate2StopLeft_vertical.size() << ',' << sumGateLeftMean3_vertical / gateValueListLeft3_vertical.size() << ',' << sumGate3StopLeft_vertical / gate3StopLeft_vertical.size() << ',' << sumGateRightMean_vertical / gateValueListRight_vertical.size() << ',' << sumGate1StopRight_vertical / gate1StopRight_vertical.size() << ',' << sumGateRightMean2_vertical / gateValueListRight2_vertical.size() << ',' << sumGate2StopRight_vertical / gate2StopRight_vertical.size() << ','  << sumGateRightMean3_vertical / gateValueListRight3_vertical.size() << ',' << sumGate3StopRight_vertical / gate3StopRight_vertical.size() << std::flush;
        std::cout << std::endl;


    }
    
    return 0;
}