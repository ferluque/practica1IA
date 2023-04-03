#include <iostream>
#include <vector>
#include <queue>

#include "include/comportamientos/comportamiento.hpp"

using namespace std;

void print_queue(queue<Action> q)
{
    while (!q.empty())
    {
        std::cout << q.front() << " ";
        q.pop();
    }
    std::cout << std::endl;
}

int main()
{
    queue<Action> plan;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        { 
            int objetivo = j;
            int brujula = i;
            int diferencia = objetivo - brujula;
            cout << "Actual: " << brujula << " // Objetivo: " << objetivo << endl; 
            if ((diferencia <= 4 and diferencia > 0) or (diferencia <= -4 and diferencia > -8))
            {
                if (diferencia < 0)
                    diferencia += 8;
                while (diferencia >= 3)
                {
                    plan.push(actTURN_BR);
                    diferencia -= 3;
                }
                while (diferencia > 0)
                {
                    plan.push(actTURN_SR);
                    diferencia--;
                }
            }
            else if (diferencia != 0)
            {
                if (diferencia > 0)
                    diferencia -= 8;
                while (diferencia <= -3 and diferencia<0)
                {
                    plan.push(actTURN_BL);
                    diferencia += 3;
                }
                while (diferencia < 0)
                {
                    plan.push(actTURN_SL);
                    diferencia++;
                }
            }
            print_queue(plan);
            queue<Action> empty;
            swap(plan, empty);
        }
    }
}