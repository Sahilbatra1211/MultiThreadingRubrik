// WebCrawlerMultiThreaded.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<bits/stdc++.h>
#include<unordered_map>
#include<thread>
#include<mutex>
using namespace std;


class HtmlParser {

public:
    vector<string> getUrls(string url) {
        
        vector<string> urls;

        if (url == "google.com") {
            urls.push_back("facebook.com"), urls.push_back("twitter.com");
        }
        else if (url == "facebook.com") {
            urls.push_back("linkedin.com");
            urls.push_back("google.com");
        }
        return urls;
    }
};

class MultiThreadedWebCrawler {
    unordered_map<string, int> vis;
    mutex mtx;
    
public:

    void executeDFS(string url, HtmlParser *htmlParser) {
        unique_lock<mutex> l(mtx);
        if (vis.count(url)) {
            return;
        }

        vis[url] = 1;

        cout << url << endl;
        l.unlock();

        vector<string> urls = htmlParser->getUrls(url);

        vector<thread> threads;

        for (auto node : urls) {
            threads.push_back(thread(&MultiThreadedWebCrawler::executeDFS,this,node, htmlParser));
        }

        for (int i = 0; i < threads.size(); i++) {
            threads[i].join();
        }
    }

};


class MultiThreadedWebCrawlerBFS {

    queue<string> q;
    unordered_map<string, int> vis;
    mutex mtx;
    condition_variable cv;
    HtmlParser* htmlParser;

public:

    MultiThreadedWebCrawlerBFS(HtmlParser* htmlParser) {
        this->htmlParser = htmlParser;
        q.push("google.com");
        vis["google.com"] = 1;
    }

    void executeBFS() {

        while (1) {
            unique_lock<mutex> l(mtx);
            cv.wait_for(l,chrono::seconds(1), [&] {return !q.empty(); });
            if (q.empty())break;

            string t = q.front();
            cout << t << endl;
            q.pop();
            l.unlock();

            vector<string> urls = htmlParser->getUrls(t);


            for (auto node : urls) {
                unique_lock<mutex> l(mtx);
                if (!vis.count(node)) {
                    vis[node] = 1;
                    q.push(node);
                }
            }
            cv.notify_all();
        }
    }

    void executeNormalBFS() {
        string t = q.front();
        cout << t << endl;
        q.pop();

        vector<string> urls = htmlParser->getUrls(t);


        for (auto node : urls) {
            if (!vis.count(node)) {
                vis[node] = 1;
                q.push(node);
            }
        }
    }

};

void runBFSThread(MultiThreadedWebCrawlerBFS *obj) {
    obj->executeBFS();
}


int main()
{
   /* MultiThreadedWebCrawler* webCrawler = new MultiThreadedWebCrawler();
    HtmlParser* htmlParser = new HtmlParser();
    webCrawler->executeDFS("google.com", htmlParser);*/

    vector<thread> threads;
    HtmlParser* htmlParser = new HtmlParser();
    MultiThreadedWebCrawlerBFS* obj = new MultiThreadedWebCrawlerBFS(htmlParser);
    for (int i = 0; i < 5; i++) {
        threads.push_back(thread(runBFSThread, obj));
    }

    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
