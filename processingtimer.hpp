#ifndef PROCESSINGTIMER_H_
#define PROCESSINGTIMER_H_

#include<iostream>
#include<chrono>
class ProcessingTimer
{
    public:
        ProcessingTimer(){
        }
        enum class Unit:int
        {
            SECONDS,
            MILLI_SECONDS,
            MICRO_SECONDS
        };
        void Start(){
            start_time_ = std::chrono::system_clock::now();
        }
        void Stop(){
            end_time_ = std::chrono::system_clock::now();
        }
        void PrintTime(Unit unit){
            switch (unit)
            {
            case Unit::SECONDS:
                elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time_-start_time_).count();
                std::cout << elapsed <<"[s]"<< std::endl;
                break;
            case Unit::MILLI_SECONDS:
                elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_-start_time_).count();
                std::cout << elapsed <<"[ms]"<< std::endl;
                break;
            case Unit::MICRO_SECONDS:
                elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time_-start_time_).count();
                std::cout << elapsed <<"[us]"<< std::endl;
                break;
            }
        }

    private:
        std::chrono::system_clock::time_point start_time_;
        std::chrono::system_clock::time_point end_time_;
        double elapsed;
};
#endif //PROCESSINGTIMER_H_