#include <bits/stdc++.h>
using namespace std;

struct EK {
    struct Edge { int to; int cap; int rev; };
    int N; vector<vector<Edge>> G;
    EK(int n=0){ init(n); }
    void init(int n){ N=n; G.assign(N+1, {}); }
    void add_dir(int u,int v,int c){ Edge a{v,c,(int)G[v].size()}, b{u,0,(int)G[u].size()}; G[u].push_back(a); G[v].push_back(b); }
    void add_und(int u,int v,int c){ add_dir(u,v,c); add_dir(v,u,c); }
    int maxflow_limited(int s,int t,int limit){ int flow=0; while(flow<limit){ vector<pair<int,int>> pre(N+1, {-1,-1}); queue<int> q; pre[s]={-2,-1}; q.push(s); while(!q.empty() && pre[t].first==-1){ int u=q.front(); q.pop(); for(int i=0;i<(int)G[u].size();++i){ auto &e=G[u][i]; if(e.cap>0 && pre[e.to].first==-1){ pre[e.to] = {u,i}; q.push(e.to); if(e.to==t) break; } } } if(pre[t].first==-1) break; int v=t; while(v!=s){ auto [u,idx] = pre[v]; auto &e = G[u][idx]; auto &rev = G[v][e.rev]; e.cap -= 1; rev.cap += 1; v = u; } ++flow; }
        return flow; }
    vector<int> reachable_from(int s){ vector<int> vis(N+1,0); queue<int> q; vis[s]=1; q.push(s); while(!q.empty()){ int u=q.front(); q.pop(); for(const auto &e:G[u]) if(e.cap>0 && !vis[e.to]){ vis[e.to]=1; q.push(e.to);} } return vis; }
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
        EK D(n);
        for(auto &e:edges){ int u=e.first, v=e.second; D.add_und(u,v,1); }
        val = D.maxflow_limited(s,t,3);
        auto vis = D.reachable_from(s);
        reach.swap(vis);
    };

    for(int i=2;i<=n;++i){
        int s=i, t=parent[i];
        vector<int> reach; long long val=0;
        build_flow(s,t,reach,val);
        w[i]=(int)val;
        for(int v=1; v<i; ++v){
            if(parent[v]==t && reach[v]) parent[v]=i;
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
