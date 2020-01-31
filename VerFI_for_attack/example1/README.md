##  Dependencies

* cd ../source/ && ./build.sh 
* NangateOpenCellLibrary_PDKv1_3_v2010_12 
```
/opt/NangateOpenCellLibrary_PDKv1_3_v2010_12/
├── Back_End
│   ├── edif
│   └── gds
└── Liberty
    └── CCS
```

##  Usage

```
make gen  # synthesize the design and fetch what we need
make sim  # run VerFI_for_attack per specifications in sim.txt
```
