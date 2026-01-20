#include <algorithm>
#include <iostream>
#include <vector>

struct Ticket {
    int id;
    int wait_minutes;
    int severity; // 1 = critical, 5 = minor
};

static std::vector<Ticket*> open_list;

Ticket* NextTicket() {
    if (open_list.empty()) return nullptr;

    auto best_it = std::min_element(
        open_list.begin(), open_list.end(),
        [](const Ticket* a, const Ticket* b) {
            int score_a = a->wait_minutes + a->severity * 10;
            int score_b = b->wait_minutes + b->severity * 10;
            return score_a < score_b; // a comes before b if its score is smaller
        }
    );

    Ticket* best = *best_it;
    open_list.erase(best_it);
    return best;
}

int main() {
    Ticket a{101,  5, 3}; // score 35
    Ticket b{102, 12, 2}; // score 32 (best first)
    Ticket c{103,  0, 4}; // score 40

    open_list = { &a, &b, &c };

    while (!open_list.empty()) {
        Ticket* t = NextTicket();
        if (!t) break;
        int score = t->wait_minutes + t->severity * 10;
        std::cout << "Serving ticket id=" << t->id << " (score=" << score << ")\n";
    }
    return 0;
}
