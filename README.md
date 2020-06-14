# Multiprocess Multithread Web Server Benchmark Analyzer
Analyzer that analyzes the difference in latency between multiprocess web server and multithread web server. Generally multihread more faster than multiprocess!

![result](https://user-images.githubusercontent.com/38535571/84592734-cad29000-ae82-11ea-8b31-01f9e3da17e0.png)
![multiprocess](https://user-images.githubusercontent.com/38535571/84592742-d7ef7f00-ae82-11ea-81df-29c20f5e022a.png)
![multithread](https://user-images.githubusercontent.com/38535571/84592748-e2aa1400-ae82-11ea-9aa0-3ac939df1dd0.png)

### Installation
```sh
$ git clone https://github.com/yoonje/multiprocess-multithread-web-server-benchmark-analyzer
```

### Address
* Multi Process: localhost:8000/index.html
* Multi Thread: localhost:8001/index.html

### Build
```sh
$ sh build.sh
```

### Run
```sh
$ sh multiprocess.sh
```
```sh
$ sh multithread.sh
```
