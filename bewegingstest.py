#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Beweegtest voor de wolken: draait een --diagnoseCsvFrames 1 dump en meet of het
wolk-veld werkelijk tussen opeenvolgende rekenrondes (ping -> pong) beweegt met
de wind, i.p.v. dat het statisch blijft staan of op zijn plek her-oplaait.

Gebruik (maak eerst een dump van opvolgende rondes):
    ./build/src/mars --procedureel --hoofdloos --diepte 4 --stappen 600 \
        --diagnoseCsvFrames 1 --diagnoseCsv /tmp/pingpong.csv
    python3 Gereedschap/bewegingstest.py /tmp/pingpong.csv

Uitvoer per tussenliggende ronde:
  - delta   : relatieve per-cel verandering van het wolk-veld t.o.v. de vorige
              ronde (0 = volkomen statisch; >0 = er verandert iets).
  - massa   : totale wolk-massa in deze ronde.
  - nieuw   : fractie van de wolk-massa die in *andere* cellen zit dan de vorige
              ronde (patroon-shift).
  - had     : aantal cellen dat zijn wolk op DEZELFDE plek houdt.

Gebruik --samenvatting <dump.csv> voor de beslissende samenvatting over de
stabiele toestand (persistentie vs. 'nieuw'). Vuistregel:
  - persistentie ~1 en delta ~0  => wolken staan stil / laaien op hun plek op.
  - persistentie laag, 'nieuw' hoog en delta duidelijk > 0 => wolken schuiven.
Vergelijk bv. --stil (bevroren) versus een normale run.
"""
import csv
import math
import sys


def groepeer(rows, per_dump):
    dumps = []
    for i in range(0, len(rows), per_dump):
        blok = rows[i:i + per_dump]
        if len(blok) == per_dump:
            w = {int(r["id"]): float(r["wolken"]) for r in blok}
            wx = {int(r["id"]): float(r["windX"]) for r in blok}
            wy = {int(r["id"]): float(r["windY"]) for r in blok}
            co = {int(r["id"]): (
                float(r["x"]), float(r["y"]), float(r["z"])) for r in blok}
            dumps.append((w, wx, wy, co))
    return dumps


def hoofd(arq):
    with open(arq, newline="") as f:
        lezer = csv.DictReader(f)
        rows = list(lezer)
    if not rows:
        print("lege dump")
        return
    # 1 identieke cel per ronde: per_dump = aantal unieke id's in dump 0
    ids = {r["id"] for r in rows}
    per_dump = len(ids)
    dumps = groepeer(rows, per_dump)
    print("cellen/ronde:", per_dump, " rondes:", len(dumps))
    print(f"{'ronde':>5} {'delta':>8} {'massa':>10} {'nieuw':>6} "
          f"{'had':>5} {'drift\u00b0/100':>11} {'drift↔wind\u00b0':>12}")
    vorig_w = None
    vorig_c = None
    voor_centr = 400  # skip opwarm-fase (wolken moeten zich eerst vormen)
    for i, (w, wx, wy, co) in enumerate(dumps):
        massa = sum(w.values())
        had = sum(1 for _ in w.values() if _ > 0.05)
        centr = centroid(w, co)
        # gemiddelde wind als richting (zonder grootte, om neutraal te wegen)
        gem_w = (sum(wx.values()) / len(wx), sum(wy.values()) / len(wy))

        if i == 0 or vorig_w is None:
            print(f"{i:>5} {'--':>8} {massa:>10.1f} {'--':>6} "
                  f"{had:>5} {'--':>11} {'--':>12}")
            vorig_w = w
            vorig_c = centr
            continue
        delta = math.sqrt(sum((w[k] - vorig_w[k]) ** 2 for k in w)) / max(massa, 1e-9)
        nieuw = sum(w[k] for k in w if vorig_w[k] <= 0.05) / max(massa, 1e-9)
        drift = "--"
        driftwind = "--"
        if i >= voor_centr and centr and vorig_c:
            drift = "%.1f" % hoek(vorig_c, centr)
            driftwind = "nvt"
        print(f"{i:>5} {delta:>8.3f} {massa:>10.1f} {nieuw:>6.2f} "
              f"{had:>5} {str(drift):>11} {str(driftwind):>12}")
        vorig_w = w
        vorig_c = centr


def centroid(w, co, drempel=0.05):
    """Gewogen midden (op de eenheidsbol) van de wolk-massa."""
    cx = cy = cz = 0.0
    tw = 0.0
    for k, val in w.items():
        if val > drempel:
            x, y, z = co[k]
            n = math.sqrt(x * x + y * y + z * z)
            cx += val * x / n
            cy += val * y / n
            cz += val * z / n
            tw += val
    if tw == 0.0:
        return None
    n = math.sqrt(cx * cx + cy * cy + cz * cz)
    return (cx / n, cy / n, cz / n)


def hoek(a, b):
    d = a[0] * b[0] + a[1] * b[1] + a[2] * b[2]
    return math.degrees(math.acos(max(-1.0, min(1.0, d))))


def hoofd(arq):
    with open(arq, newline="") as f:
        lezer = csv.DictReader(f)
        rows = list(lezer)
    if not rows:
        print("lege dump")
        return
    # 1 identieke cel per ronde: per_dump = aantal unieke id's in dump 0
    ids = {r["id"] for r in rows}
    per_dump = len(ids)
    dumps = groepeer(rows, per_dump)
    print("cellen/ronde:", per_dump, " rondes:", len(dumps))
    print(f"{'ronde':>5} {'delta':>8} {'massa':>10} {'nieuw':>6} "
          f"{'had':>5} {'drift\u00b0/100':>11} {'drift↔wind\u00b0':>12}")
    vorig_w = None
    vorig_c = None
    voor_centr = 400  # skip opwarm-fase (wolken moeten zich eerst vormen)
    for i, (w, wx, wy, co) in enumerate(dumps):
        massa = sum(w.values())
        had = sum(1 for _ in w.values() if _ > 0.05)
        centr = centroid(w, co)
        # gemiddelde wind als richting (zonder grootte, om neutraal te wegen)
        gem_w = (sum(wx.values()) / len(wx), sum(wy.values()) / len(wy))

        if i == 0 or vorig_w is None:
            print(f"{i:>5} {'--':>8} {massa:>10.1f} {'--':>6} "
                  f"{had:>5} {'--':>11} {'--':>12}")
            vorig_w = w
            vorig_c = centr
            continue
        delta = math.sqrt(sum((w[k] - vorig_w[k]) ** 2 for k in w)) / max(massa, 1e-9)
        nieuw = sum(w[k] for k in w if vorig_w[k] <= 0.05) / max(massa, 1e-9)

        # drift: verplaatsing van het wolk-centroid over dit interval (per -geef-
        # regel gemiddeld over 100 rondes), en de hoek t.o.v. de gemiddelde wind.
        drift = "--"
        driftwind = "--"
        if i >= voor_centr and centr and vorig_c:
            drift = "%.1f" % hoek(vorig_c, centr)
            # (locals raakvlak omzetting is niet exact in deze CSV; meld nvt)
            driftwind = "nvt"
        print(f"{i:>5} {delta:>8.3f} {massa:>10.1f} {nieuw:>6.2f} "
              f"{had:>5} {str(drift):>11} {str(driftwind):>12}")
        vorig_w = w
        vorig_c = centr


def samenvatting(arq):
    """Beslissende beweegtest over de stabiele toestand (laatste 100 rondes).

    - persistentie: fractie van de wolk-massa op DEZELFDE cellen als de vorige
      ronde. Statisch veld (--stil) -> ~1.0; hard schuivend -> laag.
    - nieuw: fractie op ANDERE cellen dan de vorige ronde.
    - delta: gemiddelde per-cel verandering, genormaliseerd op de massa.
    - centroid-drift: gemiddelde hoek die het wolkmidden per ronde draait.
    """
    with open(arq, newline="") as f:
        rows = list(csv.DictReader(f))
    if not rows:
        print("lege dump")
        return
    ids = {r["id"] for r in rows}
    per_dump = len(ids)
    dumps = groepeer(rows, per_dump)
    n = len(dumps)
    van, tot = max(0, n - 100), n - 1
    if van >= tot:
        print("te weinig rondes")
        return
    persist, nieuwli, delta, drift = [], [], [], []
    vorig_c = None
    for i in range(van, tot):
        w0 = dumps[i][0]
        w1 = dumps[i + 1][0]
        massa = max(sum(w1.values()), 1e-9)
        zelfde = sum(w1[k] for k in w1 if w0[k] > 0.05) / massa
        nieuw = sum(w1[k] for k in w1 if w0[k] <= 0.05) / massa
        d = math.sqrt(sum((w1[k] - w0[k]) ** 2 for k in w1)) / massa
        persist.append(zelfde)
        nieuwli.append(nieuw)
        delta.append(d)
        c = centroid(dumps[i + 1][0], dumps[i + 1][3])
        if c and vorig_c:
            drift.append(hoek(vorig_c, c))
        vorig_c = c
    print(f"\n=== BEWEEGSAMENVATTING ({arq}) ===")
    print(f"cellen/ronde={per_dump}  venster=rondes {van}..{tot}")
    print(f"  persistentie (massa op zelfde cel): {sum(persist)/len(persist):.3f}")
    print(f"  nieuw        (massa op andere cel): {sum(nieuwli)/len(nieuwli):.3f}")
    print(f"  delta        (per-cel verandering) : {sum(delta)/len(delta):.4f}")
    if drift:
        print(f"  centroid-drift per ronde          : {sum(drift)/len(drift):.2f} graden")
    print("  -> persistentie laag + nieuw hoog: wolken SCHUIVEN over de planeet")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)
    if sys.argv[1] in ("--samenvatting", "-s"):
        if len(sys.argv) < 3:
            print("gebruik: bewegingstest.py --samenvatting <dump.csv>")
            sys.exit(1)
        samenvatting(sys.argv[2])
    else:
        hoofd(sys.argv[1])
