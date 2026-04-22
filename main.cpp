#include <bits/stdc++.h>
using namespace std;

struct Dinic {
    struct Edge { int to; int cap; int rev; };
    int N;
    vector<vector<Edge>> G;
    vector<int> level, it;
    Dinic(int n=0){ init(n); }
    void init(int n){ N=n; G.assign(N+1, {}); }
    void add_edge(int u,int v,int c){
        Edge a{v,c,(int)G[v].size()};
        Edge b{u,0,(int)G[u].size()};
        G[u].push_back(a);
        G[v].push_back(b);
    }
    bool bfs(int s,int t){
        level.assign(N+1,-1);
        queue<int> q; level[s]=0; q.push(s);
        while(!q.empty()){
            int u=q.front(); q.pop();
            for(const auto &e:G[u]) if(e.cap>0 && level[e.to]<0){
                level[e.to]=level[u]+1;
                q.push(e.to);
            }
        }
        return level[t]>=0;
    }
    int dfs(int u,int t,int f){
        if(!f || u==t) return f;
        for(int &i=it[u]; i<(int)G[u].size(); ++i){
            Edge &e=G[u][i];
            if(e.cap>0 && level[e.to]==level[u]+1){
                int ret=dfs(e.to,t,min(f,e.cap));
                if(ret){ e.cap-=ret; G[e.to][e.rev].cap+=ret; return ret; }
            }
        }
        return 0;
    }
    long long maxflow(int s,int t){
        long long flow=0; const int INF=1e9;
        while(bfs(s,t)){
            it.assign(N+1,0);
            while(int f=dfs(s,t,INF)) flow+=f;
        }
        return flow;
    }
    vector<int> reachable_from(int s){
        vector<int> vis(N+1,0);
        stack<int> st; st.push(s); vis[s]=1;
        while(!st.empty()){
            int u=st.top(); st.pop();
            for(const auto &e:G[u]) if(e.cap>0 && !vis[e.to]){
                vis[e.to]=1; st.push(e.to);
            }
        }
        return vis;
    }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n,m; if(!(cin>>n>>m)) return 0;
    vector<pair<int,int>> edges; edges.reserve(m);
    for(int i=0;i<m;++i){ int a,b; cin>>a>>b; edges.emplace_back(a,b); }

    // Build Gomory-Hu tree with n-1 maxflow computations
    vector<int> parent(n+1,1), w(n+1,0);
    for(int i=2;i<=n;++i) parent[i]=1;

    auto build_flow = [&](int s, int t, vector<int> &reach, long long &val){
        Dinic D(n);
        for(auto &e:edges){ int u=e.first, v=e.second; D.add_edge(u,v,1); D.add_edge(v,u,1); }
        val = D.maxflow(s,t);
        auto vis = D.reachable_from(s);
        reach.swap(vis);
    };

    for(int i=2;i<=n;++i){
        int s=i, t=parent[i];
        vector<int> reach; long long val=0;
        build_flow(s,t,reach,val);
        w[i]=(int)val;
        for(int v=1; v<=n; ++v){
            if(v!=i && parent[v]==t && reach[v]) parent[v]=i;
        }
        if(reach[parent[t]]){
            parent[i]=parent[t];
            parent[t]=i;
            w[i]=w[t];
            w[t]=(int)val;
        }
    }

    struct E{int u,v,w;};
    vector<E> tree; tree.reserve(n-1);
    for(int i=2;i<=n;++i) tree.push_back({i,parent[i],w[i]});

    sort(tree.begin(), tree.end(), [](const E&A,const E&B){return A.w>B.w;});
    vector<int> uf(n+1); iota(uf.begin(), uf.end(), 0);
    vector<long long> sz(n+1,1);
    function<int(int)> findp = [&](int x){ return uf[x]==x?x:uf[x]=findp(uf[x]); };
    auto unite = [&](int a,int b){ a=findp(a); b=findp(b); if(a==b) return false; if(sz[a]<sz[b]) swap(a,b); uf[b]=a; sz[a]+=sz[b]; return true; };

    long long ans=0;
    for(const auto &e: tree){
        int a=findp(e.u), b=findp(e.v);
        if(a!=b){ ans += (long long)e.w * sz[a] * sz[b]; unite(a,b); }
    }

    cout<<ans<<n;
    return 0;
}
