#include <iostream>
#include <vector>
#include <algorithm>
#include <experimental/random>
#ifdef __WIN32
#define WINDOWS_SYSTEM
#include <windows.h>
#else
#include <unistd.h>
#endif


// timing function
void running_for(float time, int id){
    int t = (int) time;
    #ifdef WINDOWS_SYSTEM
        Sleep(t * 1000);
    #else
        usleep(t);
    #endif
}


// generic container to process information
struct ProcessInfo{
    int process_id_;
    float duration;
    int priority;
    bool preemptive;
    float time_to_stop;

    void content()
    {
        std::cout << "[Process: " << process_id_ << "]  duration: "<<
        duration << "s, priority: " << priority << ", interruption: " << preemptive << ", quantum: " << time_to_stop << '\n' << std::flush;

    }
};


// selection sort to order by duration
std::vector<ProcessInfo> selection_sort_order_by_duration(std::vector<ProcessInfo> p_info)  //vectors are passed by ref
{
    for(int i = 0; i < p_info.size() - 1; i++){
        for(int j = 0; j < p_info.size() - 1; j++){
           if(p_info.at(j).duration > p_info.at(j + 1).duration){
               ProcessInfo aux = p_info.at(j);
               p_info.at(j) = p_info.at(j + 1);
               p_info.at(j + 1) = aux;
           }
        }
    }
    return p_info;
}


// selection sort to order by priority
std::vector<ProcessInfo> selection_sort_order_by_priority(std::vector<ProcessInfo> p_info)  //vectors are passed by ref
{
    for(int i = 0; i < p_info.size() - 1; i++){
        for(int j = 0; j < p_info.size() - 1; j++){
            if(p_info.at(j).priority > p_info.at(j + 1).priority){
                ProcessInfo aux = p_info.at(j);
                p_info.at(j) = p_info.at(j + 1);
                p_info.at(j + 1) = aux;
            }
        }
    }
    return p_info;
}


struct WaitQueue{
    // a simple FIFO
    std::vector<ProcessInfo> wait_process_queue;

    void to_sleep(ProcessInfo p)
    {
        wait_process_queue.push_back(p);
    }

    ProcessInfo exit()
    {
        std::reverse(wait_process_queue.begin(), wait_process_queue.end());
        ProcessInfo x = wait_process_queue.at(wait_process_queue.size() - 1);
        running_for(x.duration, x.process_id_);
        wait_process_queue.pop_back();
        return x;
    }

    int length()
    {
        return wait_process_queue.size();
    }
};


class Sheduler{

public:

    // constructors
    Sheduler(int process_id, float duration, int priority, bool preemptive, float time_to_stop){
        this->process_id = process_id;
        this->duration = duration;
        this->priority = priority;
        this->preemptive = preemptive;
        this->time_to_stop = time_to_stop;
    }


    Sheduler()= default;;


    // user interaction
    void user_i()
    {
        std::vector<ProcessInfo> process_info;
        ProcessInfo pi;
        int process_count {0};
        float _duration {0};
        int _priority;
        bool _preemptive {false};

        float _time_to_stop {0};

        // questions
        std::cout << "Quantidade de processos: " << '\n' << std::flush;
        std::cin >> process_count;

        for(int process {0}; process < process_count; ++process){
            std::cout << "Tempo do processo: " << process << '\n' << std::flush;
            std::cin >> _duration;

            // random number to priority (0, 10)
            _priority = std::experimental::randint(0, 10);
            // keep run:
            std::cout << "Interrupcao do processo: " << process << "?\n" << std::flush;
            std::cin >> _preemptive;

            // quantum
            if(_preemptive)  _time_to_stop = 1;
            else _time_to_stop = 0;

            pi.process_id_ = process;
            pi.duration = _duration;
            pi.priority = _priority;
            pi.preemptive = _preemptive;
            pi.time_to_stop = _time_to_stop;
            process_info.push_back(pi);
        }


        // at ready queue
        std::cout << "[Ready queue] - FCFS " << '\n' << std::flush;
        for(ProcessInfo p : process_info){
            p.content();
        }
        std::cout.flush();
        // apply algo
       first_come_first_served_FCFS(process_info);
         std::cout << "-----------------------------------" << '\n' << std::flush;

        // at ready queue to SJF
        std::cout << "[Ready queue] - SJF " << '\n' << std::flush;
        // order by duration
        process_info = selection_sort_order_by_duration(process_info);
        for(ProcessInfo p : process_info){
            p.content();
        }
        shortest_job_first_SJF(process_info);
       

        // at ready queue to Duling
        std::cout << "[Ready queue] - Duling " << '\n' << std::flush;
        // order by priority
        process_info = selection_sort_order_by_priority(process_info);
        for(ProcessInfo p : process_info){
            p.content();
        }
        duling(process_info);
    }

    // sheduler algorithms
    void first_come_first_served_FCFS(std::vector<ProcessInfo> process_info)
    {
        std::cout << '\n' << "[FCFS]" << '\n' << std::flush;
        _first_come_first_served_FCFS(process_info);
        std::cout << '\n' << std::flush;
    }

    void shortest_job_first_SJF(std::vector<ProcessInfo> process_info)
    {
        std::cout << '\n' << "[SJF]" << '\n' << std::flush;
        _shortest_job_first_SJF(process_info);
        std::cout << '\n' << std::flush;
    }

    void duling(std::vector<ProcessInfo> process_info)
    {
        std::cout << '\n' << "[Duling]" << '\n' << std::flush;
        _shortest_job_first_SJF(process_info);
        std::cout << '\n' << std::flush;
    }

    void round_robin_RR()
    {

    }

private:
    int process_id;
    float duration;
    int priority;
    bool preemptive;
    float time_to_stop;


    WaitQueue wq;

    void _first_come_first_served_FCFS(std::vector<ProcessInfo> process_s)
    {
        // normal process queue (ready queue)
        for(ProcessInfo proc: process_s){
            if(proc.preemptive){
                proc.duration = proc.duration - proc.time_to_stop;
                if(proc.duration > 0){
                    wq.to_sleep(proc);
                }
                std::cout << "running ... Process: " << proc.process_id_ << ", duration: " << proc.time_to_stop << '\n' << std::flush;
                running_for(proc.time_to_stop, proc.process_id_); // by default, MY sheduler allow 1 sec, per execution
            }else{
                std::cout << "running ... Process: " << proc.process_id_ << ", duration: " << proc.duration - proc.time_to_stop << '\n' << std::flush;
                running_for(proc.duration - proc.time_to_stop, proc.process_id_); // by default, MY sheduler allow 1 sec, per execution
            }
        }

        // second loop, wait queue
        if(wq.length() > 0){
            std::cout << '\n' << "From [Wait Queue]" << '\n' << std::flush;
            while(wq.length() > 0){
                wq.exit().content();
            }
        }
    }

    void _shortest_job_first_SJF(std::vector<ProcessInfo> process_s)
    {
        // order by time process queue (ready queue)
        for(ProcessInfo proc: process_s){
            if(proc.preemptive){
                proc.duration = proc.duration - proc.time_to_stop;
                if(proc.duration > 0){
                    wq.to_sleep(proc);
                }
                std::cout << "running ... Process: " << proc.process_id_ << ", duration: " << proc.time_to_stop << '\n' << std::flush;
                running_for(proc.time_to_stop, proc.process_id_); // by default, MY sheduler allow 1 sec, per execution
            }else{
                std::cout << "running ... Process: " << proc.process_id_ << ", duration: " << proc.duration - proc.time_to_stop << '\n' << std::flush;
                running_for(proc.duration - proc.time_to_stop, proc.process_id_); // by default, MY sheduler allow 1 sec, per execution
            }
        }

        // second loop, wait queue
        if(wq.length() > 0){
            std::cout << '\n' << "From [Wait Queue]" << '\n' << std::flush;
            while(wq.length() > 0){
                wq.exit().content();
            }
        }
    }

    void _duling(std::vector<ProcessInfo> process_s)
    {
        // order by priority process queue (ready queue)
        for(ProcessInfo proc: process_s){
            if(proc.preemptive){
                proc.duration = proc.duration - proc.time_to_stop;
                if(proc.duration > 0){
                    wq.to_sleep(proc);
                }
                std::cout << "running ... Process: " << proc.process_id_ << ", duration: " << proc.time_to_stop << '\n' << std::flush;
                running_for(proc.time_to_stop, proc.process_id_); // by default, MY sheduler allow 1 sec, per execution
            }else{
                std::cout << "running ... Process: " << proc.process_id_ << ", duration: " << proc.duration - proc.time_to_stop << '\n' << std::flush;
                running_for(proc.duration - proc.time_to_stop, proc.process_id_); // by default, MY sheduler allow 1 sec, per execution
            }
        }

        // second loop, wait queue
        if(wq.length() > 0){
            std::cout << '\n' << "From [Wait Queue]" << '\n' << std::flush;
            while(wq.length() > 0){
                wq.exit().content();
            }
        }
    }

    void _round_robin_RR()
    {

    }

};


int main()
{
    std::ios_base::sync_with_stdio(false);

    Sheduler sheduler{};
    sheduler.user_i();

    return EXIT_SUCCESS;
}
