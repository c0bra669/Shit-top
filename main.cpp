#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<unistd.h>
#include<vector>
#include<cmath>
void draw_bar(float prec);
class Cpu{
public:
    std::string id;
    u_int64_t user;
    u_int64_t nice;
    u_int64_t system;
    u_int64_t idle;
    u_int64_t iowait;
    u_int64_t irq;
    u_int64_t softirq;
    u_int64_t steal;
    u_int64_t guest;
    u_int64_t guest_nice;
public:
    float calc_usage(const Cpu & prev_cpu){
        u_int64_t cur_idle=
            this -> idle +
            this -> iowait;
        u_int64_t cur_busy=
            this -> user +
            this -> nice +
            this -> system +
            this -> irq +
            this -> softirq +
            this -> steal +
            this -> guest +
            this ->guest_nice;
        u_int64_t prev_busy=
            prev_cpu.user+
            prev_cpu.nice+
            prev_cpu.system+
            prev_cpu.irq+
            prev_cpu.softirq+
            prev_cpu.steal+
            prev_cpu.guest+
            prev_cpu.guest_nice;
        u_int64_t prev_idle=
            prev_cpu.idle+
            prev_cpu.iowait;
        if(cur_idle < prev_idle || cur_busy < prev_busy){
            return 0.0f;
        }
        u_int64_t idle=cur_idle-prev_idle;
        u_int64_t busy=cur_busy-prev_busy;
        u_int64_t total=idle+busy;
        if (total==0){
            return 0.0f;
        }
        return 100.0f * static_cast<float>(busy) / total;
    }
    void data_from_string(std::string &line){
        std::istringstream iss(line);
        iss >> this->id
            >> this->user
            >> this->nice
            >> this->system
            >> this->idle
            >> this->iowait
            >> this->irq
            >> this->softirq
            >> this->steal
            >> this->guest
            >> this->guest_nice;
    }

};
class Mem{
public:
    u_int64_t mem_total{};
    u_int64_t mem_free{};
    u_int64_t mem_available{};
    u_int64_t swap{};
private:
    float free_prec{};
    float ava_prec{};
public:
    void get_dat(){
        std::ifstream file("/proc/meminfo");
        std::string key;
        long value;
        std::string unit;
        while (file >> key >> value >> unit) {
            if (key == "MemTotal:")
                this -> mem_total = value;
            else if (key == "MemFree:")
                this -> mem_free = value;
            else if (key == "MemAvailable:")
                this -> mem_available = value;
            else if (key == "SwapTotal:")
                this -> swap = value;
            }
        file.close();
    }
    void print() {
        calc_prec();

        std::cout << "MemTotal: " << mem_total << "\n";

        std::cout << "MemFree: ";
        draw_bar(free_prec);
        std::cout << mem_free << "\n";

        std::cout << "MemAvailable: ";
        draw_bar(ava_prec);
        std::cout << mem_available << "\n";

        std::cout << "SwapTotal: " << swap << "\n";
    }
private:
    void calc_prec(){
        if(mem_total==0){
            this -> free_prec = 0.0f;
            this -> ava_prec  = 0.0f;
        }
        this -> free_prec= static_cast<float>(this -> mem_free) / static_cast<float>(this -> mem_total) *100.0f;
        this -> ava_prec= static_cast<float>(this -> mem_available) / static_cast<float>(this -> mem_total) * 100.0f;
    }

};
void draw_bar(float prec){
    char bar_char='|';
    int length=20;
    int filled = static_cast<int>(std::floor((prec/100.0f) * length));
    std::cout << '['
          << std::string(filled, '|')
          << std::string(length - filled, ' ')
          << "]";

}
std::vector<Cpu>read_proc(){
    std::vector<Cpu>data;
    std::ifstream file("/proc/stat");
    std::string line;
    while (std::getline(file, line)) {

        // Only lines starting with "cpu"
        if (line.rfind("cpu", 0) != 0)
            break;

        //std::istringstream iss(line);
        Cpu cpu;
        cpu.data_from_string(line);
        // iss >> cpu.id
        //     >> cpu.user
        //     >> cpu.nice
        //     >> cpu.system
        //     >> cpu.idle
        //     >> cpu.iowait
        //     >> cpu.irq
        //     >> cpu.softirq
        //     >> cpu.steal
        //     >> cpu.guest
        //     >> cpu.guest_nice;

       data.push_back(cpu);
    }
    
    file.close();
    return data;
}
int main(int argc,char **argv){
    std::vector<Cpu> curr_cpus;
    std::vector<Cpu> prev_cpus;
    curr_cpus=read_proc();
    Mem mem;
 sleep(1);
 while (true)
 {
     std::cout<<"\033[H\033[J";
     prev_cpus=curr_cpus;
     curr_cpus=read_proc();
     for (int i = 0; i < curr_cpus.size(); ++i) {
        float u = curr_cpus[i].calc_usage(prev_cpus[i]);
        std::cout<< curr_cpus[i].id;
        draw_bar(u);
        std::cout<<u<<"%\n";
     }
     mem.get_dat();
     mem.print();
     sleep(1);
 }
  
 
 return 0;
}
