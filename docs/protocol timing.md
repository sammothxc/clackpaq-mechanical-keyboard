# Protocol Timing

t is for time, c for clk, d for data h for high, l for low.
before t=0, c and d idle at h. 
t resolution is +/- 2us

**<kbd>A</kbd> Make (Keypress)**

```
t=0: c goes l
t=42us: c goes h, d goes l
t=56us: c gpes l
t=82us: c goes h
t=94us: c goes l
t=120us: c goes h, d goes h
t=134us: c goes l
t=160us: c goes h
t=172us: c goes l
t=198us: c goes h
t=212us: c goes l
t=238us: c goes h, d goes l
t=250us: c goes l
t=276us: c goes h
t=290us: c goes l
t=316us: c goes h
t=330us: c goes l
t=358us: c goes h, and will remain at h at idle
t=999us: d goes h, and will remain at h at idle
```

**<kbd>A</kbd> Break (Key Release)**

```
t=0: c goes l
t=42us: c goes h, d goes l
t=56us: c gpes l
t=82us: c goes h
t=94us: c goes l
t=120us: c goes h, d goes h
t=134us: c goes l
t=160us: c goes h
t=172us: c goes l
t=198us: c goes h
t=212us: c goes l
t=238us: c goes h, d goes l
t=250us: c goes l
t=276us: c goes h
t=290us: c goes l
t=316us: c goes h, d goes h
t=330us: c goes l, d goes l
t=358us: c goes h, and will remain at h at idle
t=858us: d goes h, and will remain at h at idle
```