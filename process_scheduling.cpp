#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <fstream>
#include <iomanip>
using namespace std;

// ���̽ṹ��
struct Process {
    int id;             // ����ID
    int arrival_time;   // ����ʱ��
    int burst_time;     // ͻ��ʱ��
    int priority;       // ���ȼ�
    int remaining_time; // ʣ������ʱ��
    int completion_time;// ���ʱ��
    int turnaround_time;// ��תʱ��
    int waiting_time;   // �ȴ�ʱ��
    int response_time;  // ��Ӧʱ��
    bool started;       // �Ƿ�ʼִ��

    Process(int _id, int _arrival, int _burst, int _priority) {
        id = _id;
        arrival_time = _arrival;
        burst_time = _burst;
        priority = _priority;
        remaining_time = burst_time;
        completion_time = 0;
        turnaround_time = 0;
        waiting_time = 0;
        response_time = -1;
        started = false;
    }
};

// ����ָ��ṹ��
struct Metrics {
    string algorithm;
    double avg_turnaround;
    double avg_waiting;
    double avg_response;
    double cpu_utilization;
    double throughput;
};

// ��ӡ����������ָ��
Metrics printMetrics(const vector<Process>& processes, int total_time, const string& algorithm_name) {
    double avg_turnaround = 0, avg_waiting = 0, avg_response = 0;
    double total_burst = 0;
    int n = processes.size();

    cout << "\nProcess Execution Details:" << endl;
    cout << "PID\tAT\tBT\tCT\tTAT\tWT\tRT" << endl;

    for (const Process& p : processes) {
        cout << p.id << "\t"
            << p.arrival_time << "\t"
            << p.burst_time << "\t"
            << p.completion_time << "\t"
            << p.turnaround_time << "\t"
            << p.waiting_time << "\t"
            << p.response_time << endl;

        avg_turnaround += p.turnaround_time;
        avg_waiting += p.waiting_time;
        avg_response += p.response_time;
        total_burst += p.burst_time;
    }

    avg_turnaround /= n;
    avg_waiting /= n;
    avg_response /= n;
    double cpu_utilization = (total_burst / total_time) * 100;
    double throughput = (double)n / total_time;

    cout << "\nPerformance Metrics:" << endl;
    cout << "Average Turnaround Time: " << avg_turnaround << endl;
    cout << "Average Waiting Time: " << avg_waiting << endl;
    cout << "Average Response Time: " << avg_response << endl;
    cout << "CPU Utilization: " << cpu_utilization << "%" << endl;
    cout << "Throughput: " << throughput << " processes per unit time" << endl;

    return Metrics{ algorithm_name, avg_turnaround, avg_waiting, avg_response,
                  cpu_utilization, throughput };
}

// FCFS�㷨
Metrics FCFS(vector<Process>& processes) {
    sort(processes.begin(), processes.end(),
        [](const Process& a, const Process& b) {
            return a.arrival_time < b.arrival_time;
        });

    int current_time = 0;
    for (auto& p : processes) {
        if (current_time < p.arrival_time)
            current_time = p.arrival_time;

        p.response_time = current_time - p.arrival_time;
        p.completion_time = current_time + p.burst_time;
        p.turnaround_time = p.completion_time - p.arrival_time;
        p.waiting_time = p.turnaround_time - p.burst_time;

        current_time = p.completion_time;
    }

    return printMetrics(processes, current_time, "FCFS");
}

// SJF�㷨
Metrics SJF(vector<Process>& processes) {
    vector<Process> ready_queue;
    vector<Process> temp = processes;
    int current_time = 0;
    int completed = 0;
    int n = processes.size();

    while (completed != n) {
        for (auto it = temp.begin(); it != temp.end();) {
            if (it->arrival_time <= current_time) {
                ready_queue.push_back(*it);
                it = temp.erase(it);
            }
            else {
                ++it;
            }
        }

        if (ready_queue.empty()) {
            current_time++;
            continue;
        }

        auto shortest = min_element(ready_queue.begin(), ready_queue.end(),
            [](const Process& a, const Process& b) {
                return a.burst_time < b.burst_time;
            });

        shortest->response_time = current_time - shortest->arrival_time;
        shortest->completion_time = current_time + shortest->burst_time;
        shortest->turnaround_time = shortest->completion_time - shortest->arrival_time;
        shortest->waiting_time = shortest->turnaround_time - shortest->burst_time;

        current_time = shortest->completion_time;

        for (auto& p : processes) {
            if (p.id == shortest->id) {
                p = *shortest;
                break;
            }
        }

        ready_queue.erase(shortest);
        completed++;
    }

    return printMetrics(processes, current_time, "SJF");
}

// SRTF�㷨
Metrics SRTF(vector<Process>& processes) {
    vector<Process> temp = processes;
    int n = processes.size();
    int completed = 0;
    int current_time = 0;
    vector<bool> is_completed(n, false);

    while (completed != n) {
        int shortest_remaining_time = INT_MAX;
        int shortest_idx = -1;

        for (int i = 0; i < n; i++) {
            if (temp[i].arrival_time <= current_time &&
                !is_completed[i] &&
                temp[i].remaining_time < shortest_remaining_time &&
                temp[i].remaining_time > 0) {
                shortest_remaining_time = temp[i].remaining_time;
                shortest_idx = i;
            }
        }

        if (shortest_idx == -1) {
            current_time++;
            continue;
        }

        if (!temp[shortest_idx].started) {
            temp[shortest_idx].response_time = current_time - temp[shortest_idx].arrival_time;
            temp[shortest_idx].started = true;
        }

        temp[shortest_idx].remaining_time--;
        current_time++;

        if (temp[shortest_idx].remaining_time == 0) {
            temp[shortest_idx].completion_time = current_time;
            temp[shortest_idx].turnaround_time = temp[shortest_idx].completion_time -
                temp[shortest_idx].arrival_time;
            temp[shortest_idx].waiting_time = temp[shortest_idx].turnaround_time -
                temp[shortest_idx].burst_time;

            is_completed[shortest_idx] = true;
            completed++;

            for (auto& p : processes) {
                if (p.id == temp[shortest_idx].id) {
                    p = temp[shortest_idx];
                    break;
                }
            }
        }
    }

    return printMetrics(processes, current_time, "SRTF");
}

// RR�㷨
Metrics RR(vector<Process>& processes, int quantum) {
    vector<Process> temp = processes;
    queue<int> ready_queue;
    int n = processes.size();
    int current_time = 0;
    vector<int> remaining_time(n);

    for (int i = 0; i < n; i++) {
        remaining_time[i] = temp[i].burst_time;
    }

    int completed = 0;
    vector<bool> in_queue(n, false);

    while (completed != n) {
        for (int i = 0; i < n; i++) {
            if (temp[i].arrival_time <= current_time && !in_queue[i] && remaining_time[i] > 0) {
                ready_queue.push(i);
                in_queue[i] = true;
            }
        }

        if (ready_queue.empty()) {
            current_time++;
            continue;
        }

        int idx = ready_queue.front();
        ready_queue.pop();
        in_queue[idx] = false;

        if (!temp[idx].started) {
            temp[idx].response_time = current_time - temp[idx].arrival_time;
            temp[idx].started = true;
        }

        if (remaining_time[idx] <= quantum) {
            current_time += remaining_time[idx];
            remaining_time[idx] = 0;

            temp[idx].completion_time = current_time;
            temp[idx].turnaround_time = temp[idx].completion_time - temp[idx].arrival_time;
            temp[idx].waiting_time = temp[idx].turnaround_time - temp[idx].burst_time;

            for (auto& p : processes) {
                if (p.id == temp[idx].id) {
                    p = temp[idx];
                    break;
                }
            }

            completed++;
        }
        else {
            current_time += quantum;
            remaining_time[idx] -= quantum;

            for (int i = 0; i < n; i++) {
                if (temp[i].arrival_time <= current_time && !in_queue[i] && remaining_time[i] > 0) {
                    ready_queue.push(i);
                    in_queue[i] = true;
                }
            }

            ready_queue.push(idx);
            in_queue[idx] = true;
        }
    }

    return printMetrics(processes, current_time, "RR");
}

// ���ȼ������㷨
Metrics PriorityScheduling(vector<Process>& processes) {
    vector<Process> temp = processes;
    int n = processes.size();
    int completed = 0;
    int current_time = 0;
    vector<bool> is_completed(n, false);

    while (completed != n) {
        int highest_priority = INT_MAX;
        int selected_idx = -1;

        for (int i = 0; i < n; i++) {
            if (temp[i].arrival_time <= current_time &&
                !is_completed[i] &&
                temp[i].priority < highest_priority) {
                highest_priority = temp[i].priority;
                selected_idx = i;
            }
        }

        if (selected_idx == -1) {
            current_time++;
            continue;
        }

        if (!temp[selected_idx].started) {
            temp[selected_idx].response_time = current_time - temp[selected_idx].arrival_time;
            temp[selected_idx].started = true;
        }

        current_time += temp[selected_idx].burst_time;
        temp[selected_idx].completion_time = current_time;
        temp[selected_idx].turnaround_time = temp[selected_idx].completion_time -
            temp[selected_idx].arrival_time;
        temp[selected_idx].waiting_time = temp[selected_idx].turnaround_time -
            temp[selected_idx].burst_time;

        is_completed[selected_idx] = true;
        completed++;

        for (auto& p : processes) {
            if (p.id == temp[selected_idx].id) {
                p = temp[selected_idx];
                break;
            }
        }
    }

    return printMetrics(processes, current_time, "Priority");
}

// ����CSV�ļ��ĺ���
void generateCSV(const vector<Metrics>& all_metrics) {
    string filename = "Comparison_Chart.csv";
    ofstream csv_file(filename);

    // д���ͷ
    csv_file << "Algorithm,Average Turnaround Time,Average Waiting Time,Average Response Time,"
        << "CPU Utilization (%),Throughput (processes/unit time)" << endl;

    // д������
    for (const auto& metric : all_metrics) {
        csv_file << metric.algorithm << ","
            << fixed << setprecision(2)
            << metric.avg_turnaround << ","
            << metric.avg_waiting << ","
            << metric.avg_response << ","
            << metric.cpu_utilization << ","
            << metric.throughput << endl;
    }

    csv_file.close();
    cout << "\nMetrics have been saved to " << filename << endl;
}

int main() {
    vector<Metrics> all_metrics;  // �洢�����㷨������ָ��

    // ����4�����̵����ݼ�
    vector<Process> processes = {
        Process(1, 0, 6, 2), 
        Process(2, 2, 4, 1),  
        Process(3, 4, 8, 3),  
        Process(4, 6, 3, 4)   
    };

    cout << "\n=== First Come First Serve (FCFS) ===" << endl;
    vector<Process> fcfs_processes = processes;
    all_metrics.push_back(FCFS(fcfs_processes));

    cout << "\n=== Shortest Job First (SJF) ===" << endl;
    vector<Process> sjf_processes = processes;
    all_metrics.push_back(SJF(sjf_processes));

    cout << "\n=== Shortest Remaining Time First (SRTF) ===" << endl;
    vector<Process> srtf_processes = processes;
    all_metrics.push_back(SRTF(srtf_processes));

    cout << "\n=== Round Robin (RR) with Time Quantum = 2 ===" << endl;
    vector<Process> rr_processes = processes;
    all_metrics.push_back(RR(rr_processes, 2));

    cout << "\n=== Priority Scheduling ===" << endl;
    vector<Process> priority_processes = processes;
    all_metrics.push_back(PriorityScheduling(priority_processes));

    // ����CSV�ļ�
    generateCSV(all_metrics);

    return 0;
}