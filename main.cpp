#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

class Segment{
     int num;
     std::pair<double,double> a;
     std::pair<double,double> b;
    int status ;
public:
    Segment(int _num, std::pair<double,double> _a, std::pair<double,double> _b){
        a = _a;
        b = _b;
        num = _num;
    }

    void setStatus(int num){
        status = num;
    }
     std::pair<double,double>getPointA(){
        return a;
    }
     std::pair<double,double>getPointB(){
        return b;
    }
    int getNum () {
        return num;
    }
    int getStatus (){
        return status;
    }
};

std::pair<double,double> parsingLine(std::string line){
    std::pair<double,double>point;
    size_t pos;
    pos = line.find_first_of(",");
    point.first = std::stod(line.substr(0,pos));
    point.second = std::stod(line.substr((pos+1),line.size() - pos));
 return point;

}
bool testPoint(std::pair<double, double> point,std::vector<std::pair<double, double>> rectPic){
    bool flag = true;
    for(int i = 0;i < rectPic.size(); i++){
        if(i == 0){
            if(((rectPic.back().first - point.first) * (rectPic[i].second - rectPic.back().second)
            - (rectPic.back().second - point.second) * (rectPic[i].first - rectPic.back().first)) > 0) flag = false;
        } else{
            if((rectPic[i - 1].first - point.first) * (rectPic[i].second - rectPic[i - 1].second)
               - (rectPic[i - 1].second - point.second) * (rectPic[i].first - rectPic[i - 1].first) > 0) flag = false;
        }
    }
    return flag;
}

bool crossSegments (std::pair<double, double>sideA, std::pair<double, double>sideB, std::pair<double, double>segA, std::pair<double, double>segB){
    double k1,k2,b1,b2,x; //уравнение прямой y = kx + b ; k = (y2 - y1)/(x2 - x1) ; b = y1 - k*x1;
    if(sideB.first - sideA.first !=0) {
        k1 = (sideB.second - sideA.second) / (sideB.first - sideA.first);
    }
    else{
        k1 = sideB.second - sideA.second;
    }
    b1 = sideA.second - k1 * sideA.first;
    if(segB.first - segA.first !=0) {
        k2 = (segB.second - segA.second) / (segB.first - segA.first);
    }
    else {
        k2 = segB.second - segA.second;
    }
    b2 = segA.second - k2 * segA.first;
    x = (b2 - b1)/(k1 - k2);
    if ( sideB.first > sideA.first && x <= sideB.first && x >= sideA.first
         || sideB.first < sideA.first && x >= sideB.first && x <= sideA.first ) return true;
    return false;
}

int main(int argc,char *argv[]) {
    std::filesystem::path p (argv[0]);
    std::filesystem::path p2 = p.parent_path().parent_path();
    std::vector<Segment>vec_segments;
    double Ax, Ay, Bx, By, Cx, Cy;
    std::string string_line;
    std::pair<double, double> point;
    std::vector<std::pair<double, double>> rectPic;
    std::ifstream polys(p2 /= "polys.txt");
    while (std::getline(polys, string_line)) {
        std::size_t pos, prev = 0;
        while ((pos = string_line.find_first_of(" ", prev)) != std::string::npos) {
            if (pos > prev) {
                std::string str = string_line.substr(prev, pos - prev);
                rectPic.push_back(parsingLine(str));
                prev = pos + 1;
            } else if (pos == prev)prev = pos + 1;
        }
        if (prev < string_line.length()) {
            rectPic.push_back(parsingLine(string_line.substr(prev, std::string::npos)));
        }
    }
    polys.close();
    std::ifstream lines(p2.parent_path() /= "lines.txt");
    bool s = false;
    if(lines.is_open()) s= true;
    int count = 1;
    while (std::getline(lines, string_line)) {
        size_t pos;
        pos = string_line.find_first_of(" ");
        Segment segment(count,parsingLine(string_line.substr(0,pos)),parsingLine(string_line.substr(pos + 1,string_line.size() - pos)));
        vec_segments.push_back(segment);
        count ++;
    }
    lines.close();
    for(auto &item:vec_segments){
       bool testA = testPoint(item.getPointA(), rectPic);
       bool testB = testPoint(item.getPointB(), rectPic);
       if(testA && testB){
           item.setStatus(0);
       }
       else if((testA && !testB) || (!testA && testB)){
           item.setStatus(2);
       }
       else{
           bool flag = false;
           for(int i = 0; i < rectPic.size(); i++){
               if(i == 0){
                   if(crossSegments(rectPic.back(),rectPic[i],item.getPointA(),item.getPointB()))flag = true;
               }
               else{
                   if(crossSegments(rectPic[i - 1],rectPic[i],item.getPointA(),item.getPointB())) flag = true;
               }
           }
           if(flag) item.setStatus(3);
           else item.setStatus(1);
       }
    }
    std::ofstream result(p2.parent_path() /= "result.txt");
    for (auto item: vec_segments ){
        result << "Segment # " << item.getNum() << " status "<<item.getStatus()<<std::endl;
    }
    result.close();
    return 0;
}
