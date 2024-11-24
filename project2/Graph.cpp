#include "Graph.h"

Graph::Graph(const char *const &edgelist_csv_fn) {
    ifstream file(edgelist_csv_fn);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string u, v;
        double w;
        getline(ss, u, ',');
        getline(ss, v, ',');
        ss >> w;

        edges.emplace_back(u, v, w);
        m_nodes.insert(u);
        m_nodes.insert(v);
    }
    sort(edges.begin(), edges.end(), tuple_compare);
    file.close();
}

unsigned int Graph::num_nodes() {
    return m_nodes.size();
}

vector<string> Graph::nodes() {
    return vector<string>{m_nodes.begin(), m_nodes.end()};
}

unsigned int Graph::num_edges() {
    return edges.size();
}

unsigned int Graph::num_neighbors(string const &node_label) {
    int count = 0;
    for (auto &tp: edges) {
        if (get<0>(tp) == node_label || get<1>(tp) == node_label) count++;
    }
    return count;
}

double Graph::edge_weight(string const &u_label, string const &v_label) {
    for (auto &tp: edges) {
        if (get<0>(tp) == u_label && get<1>(tp) == v_label) return get<2>(tp);
        if (get<0>(tp) == v_label && get<1>(tp) == u_label) return get<2>(tp);
    }
    return -1;
}

vector<string> Graph::neighbors(string const &node_label) {
    unordered_set<string> ans;
    for (auto &tp: edges) {
        if (get<0>(tp) == node_label) ans.insert(get<1>(tp));
        if (get<1>(tp) == node_label) ans.insert(get<0>(tp));
    }
    return vector<string>{ans.begin(), ans.end()};
}

vector<string> Graph::shortest_path_unweighted(string const &start_label, string const &end_label) {
    unordered_map<string, string> prev;
    unordered_set<string> visit;
    vector<string> path;
    queue<string> q;

    q.push(start_label);
    visit.insert(start_label);
    while (!q.empty()) {
        string curr = q.front();
        q.pop();
        if (curr == end_label) {
            while (curr != start_label) {
                path.push_back(curr);
                curr = prev[curr];
            }
            path.push_back(start_label);
            reverse(path.begin(), path.end());
            return path;
        }
        vector<string> currNeighbors = neighbors(curr);

        for (auto &neighbor: currNeighbors) {
            if (!visit.count(neighbor)) {
                visit.insert(neighbor);
                prev[neighbor] = curr;
                q.push(neighbor);
            }
        }
    }
    return {};
}

vector<tuple<string, string, double>> Graph::shortest_path_weighted(string const &start_label, string const &end_label) {
    if (start_label == end_label) {
        return {make_tuple(start_label, start_label, -1)};
    }
    unordered_map<string, string> prev;
    unordered_set<string> visit;
    unordered_map<string, double> dist;
    vector<tuple<string, string, double>> path;
    priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;

    pq.push({0, start_label});

    while (!pq.empty()) {
        string curr = pq.top().second;
        double value = pq.top().first;
        pq.pop();
        if (visit.count(curr)) continue;
        visit.insert(curr);
        if (curr == end_label) {
            while (curr != start_label) {
                path.emplace_back(prev[curr], curr, edge_weight(prev[curr], curr));
                curr = prev[curr];
            }
            reverse(path.begin(), path.end());
            return path;
        }
        vector<string> currNeighbors = neighbors(curr);

        for (auto &neighbor: currNeighbors) {
            if (!visit.count(neighbor) && (!dist.count(neighbor) || dist[neighbor] > value + edge_weight(curr, neighbor))) {
                dist[neighbor] = value + edge_weight(curr, neighbor);
                prev[neighbor] = curr;
                pq.push({dist[neighbor], neighbor});
            }
        }
    }
    return {};
}

vector<vector<string>> Graph::connected_components(double const &threshold) {
    vector<vector<string>> components;
    unordered_set<string> visit;
    vector<string> total_nodes = nodes();
    for (auto &node: total_nodes) {
        if (visit.count(node)) continue;
        queue<string> q;
        q.push(node);
        visit.insert(node);

        vector<string> component;
        component.push_back(node);

        while (!q.empty()) {
            string curr = q.front();
            q.pop();
            vector<string> currNeighbors = neighbors(curr);
            for (auto &neighbor: currNeighbors) {
                if (edge_weight(neighbor, curr) > threshold) continue;
                if (!visit.count(neighbor)) {
                    visit.insert(neighbor);
                    component.push_back(neighbor);
                    q.push(neighbor);
                }
            }
        }
        components.push_back(component);
    }
    return components;
}

string Graph::get_set(std::string curr) {
    if (Disjoint[curr] == curr) return curr;
    return curr = get_set(Disjoint[curr]);
}

void Graph::merge_set(std::string p, std::string q) {
    p = get_set(p);
    q = get_set(q);
    Disjoint[p] = q;
}

double Graph::smallest_connecting_threshold(string const &start_label, string const &end_label) {
    if (start_label == end_label) return 0;
    Disjoint.clear();
    vector<string> total_nodes = nodes();

    for (auto &node: total_nodes) {
        Disjoint[node] = node;
    }

    for (auto & edge : edges) {
        merge_set(get<0>(edge), get<1>(edge));
        if (get_set(start_label) == get_set(end_label)) {
            return get<2>(edge);
        }
    }

    return -1;
}

bool Graph::tuple_compare(const tuple<std::string, std::string, double> &p, const tuple<std::string, std::string, double> &q) {
    return get<2>(p) < get<2>(q);
}

