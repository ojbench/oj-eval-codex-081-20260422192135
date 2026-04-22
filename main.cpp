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

    // Build cut tree using Stoer–Wagner phases
    int N = n;
    vector<vector<int>> W(N, vector<int>(N, 0));
    for(auto &e: edges){ int u=e.first-1, v=e.second-1; ++W[u][v]; ++W[v][u]; }
    vector<int> verts(N); iota(verts.begin(), verts.end(), 0);
    vector<int> parent_idx(N, -1), weight_cut(N, 0);
    while((int)verts.size() > 1){
        int m = verts.size();
        vector<int> added(N, 0), wsum(N, 0);
        int s = verts[0], t = -1;
        for(int itc=0; itc<m; ++itc){
            int sel = -1;
            for(int v: verts){ if(!added[v] && (sel==-1 || wsum[v] > wsum[sel])) sel=v; }
            if(sel==-1) break;
            added[sel] = 1;
            if(itc == m-1){
                t = sel;
                break;
            }
            s = sel;
            for(int v: verts){ if(!added[v]) wsum[v] += W[sel][v]; }
        }
        int cutval = 0;
        for(int v: verts) if(v!=t) cutval += W[t][v];
        parent_idx[t] = s;
        weight_cut[t] = cutval;
        // merge t into s
        for(int v: verts){ if(v!=s && v!=t){ W[s][v] += W[t][v]; W[v][s] = W[s][v]; } }
        // remove t from verts
        verts.erase(find(verts.begin(), verts.end(), t));
    }

    struct E{int u,v,w;};
    vector<E> tree; tree.reserve(n-1);
    for(int v=0; v<N; ++v){ if(parent_idx[v] != -1){ tree.push_back({v+1, parent_idx[v]+1, weight_cut[v]}); } }

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
