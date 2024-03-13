## Benchmarks


| *                 | BM_CDR     | BM_OKX     | BM_BNN     | Slowdown (Avg) |
|-------------------|------------|------------|------------|----------------|
| Default           | 599.888us  | 92.072us   | 130.615us  | 1x             |
| Dynamic           | 530.134us  | 94.164us   | 102.539us  | 0.98x          |
| RingPreInitOnly   | 530.134us  | 88.933us   | 108.817us  | 0.99x          |
| Format            | 5580.36us  | 669.643us  | 655.692us  | 10.43x         |
| Stream            | 8159.72us  | 2949.3us   | 3928.07us  | 20.25x         |
| Perfect           | 531.25us   | 87.193us   | 107.422us  | 0.99x          |

