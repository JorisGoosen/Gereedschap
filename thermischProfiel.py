#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Thermisch/gliederingsprofiel van de atmosfeer (T en wind).

Leest een --diagnoseCsv dump (één of meerdere rondes; gebruikt de LAATSTE ronde)
en scoret objectief hoe "geclusterd" vs. "glad" de temperatuur- en windvelden
zijn. Dit beantwoordt de vraag of het vreemde patroon in de T- en V-overlays
komt van (a) een te breed temperatuurbereik, (b) te scherpe randen, of (c) te
gefragmenteerde cellen.

Gebruik (maak eerst een dump):
    ./build/src/mars --procedureel --hoofdloos --diepte 4 --stappen 400 \
        --diagnoseCsvFrames 1 --diagnoseCsv /tmp/profiel.csv
    python3 Gereedschap/thermischProfiel.py /tmp/profiel.csv

Rapporteert:
  - blauw/groen/rood : fractie cellen resp. koud(<248.15K), midden, heet(>298.15K)
                       (zelfde drempels als de temperatuuroverlay; blauw+rood groot = bimodaal)
  - aangrenzende |dT|: gemiddelde en p95 over cel-buur-randen (hoog = scherpe randen)
  - patches          : aantal samenhangende clusters van extreme cellen (= rood of blauw)
                       + grootte van de grootste. 1-2 grote helften = goed dag/nacht;
                       veel kleine = gefragmenteerde clusters.
  - wind             : verdeling |wind| en aangrenzende |dWind| (patchiness van V).
"""
import csv
import math
import sys


def groepeer(rows, per_dump):
    dumps = []
    for i in range(0, len(rows), per_dump):
        blok = rows[i:i + per_dump]
        if len(blok) == per_dump:
            dumps.append(blok)
    return dumps


def main(arq):
    with open(arq, newline="") as f:
        rows = list(csv.DictReader(f))
    if not rows:
        print("lege dump")
        return 2

    per_dump = len({r["id"] for r in rows})
    dumps = groepeer(rows, per_dump)
    blok = dumps[-1]  # de laatste (meest geëvolueerde) ronde
    n = len(blok)

    P = []   # genormaliseerde richtingen
    T = []   # temperatuur
    W = []   # windsnelheid |wind|
    for r in blok:
        x, y, z = float(r["x"]), float(r["y"]), float(r["z"])
        L = math.sqrt(x * x + y * y + z * z) or 1.0
        P.append((x / L, y / L, z / L))
        T.append(float(r["temperatuur"]))
        W.append(math.hypot(float(r["windX"]), float(r["windY"])))

    # buren: de 7 dichtstbijzijnde andere cellen op de bol (superset van de ~6 echte buren)
    buren = [[] for _ in range(n)]
    for i in range(n):
        scores = []
        for j in range(n):
            if i == j:
                continue
            d = (P[i][0] * P[j][0] + P[i][1] * P[j][1] + P[i][2] * P[j][2])
            scores.append((d, j))
        scores.sort(reverse=True)
        buren[i] = [j for _, j in scores[:7]]

    def median_percentiel(vals, p):
        s = sorted(vals)
        k = max(0, min(len(s) - 1, int(round((p / 100.0) * (len(s) - 1)))))
        return s[k]

    # 1) temperatuurverdeling in overlay-buckets
    blauw = sum(1 for t in T if t < 248.15)
    groen = sum(1 for t in T if 248.15 <= t <= 298.15)
    rood = sum(1 for t in T if t > 298.15)
    print("\n=== THERMISCH PROFIEL (ronde %d, %d cellen) ===" % (len(dumps), n))
    print("  T bereik      : %.1f .. %.1f K (%.1f .. %.1f °C)" %
          (min(T), max(T), min(T) - 273.15, max(T) - 273.15))
    print("  overlay       : blauw=%.1f%%  groen=%.1f%%  rood=%.1f%%  (blauw+rood=%.1f%%)" %
          (100.0 * blauw / n, 100.0 * groen / n, 100.0 * rood / n,
           100.0 * (blauw + rood) / n))

    # 2) aangrenzende |dT|
    dT = []
    dW = []
    for i in range(n):
        for j in buren[i]:
            if j > i:
                dT.append(abs(T[i] - T[j]))
                dW.append(abs(W[i] - W[j]))
    print("  aangrenzende |dT|: gem=%.2f K  p95=%.2f K  (scherpte)" %
          (sum(dT) / len(dT), median_percentiel(dT, 95)))
    print("  aangrenzende |dW|: gem=%.3f  p95=%.3f  (wind-patchiness)" %
          (sum(dW) / len(dW), median_percentiel(dW, 95)))

    # 3) samenhangende componenten van extreme cellen (rood of blauw)
    extreem = [t < 248.15 or t > 298.15 for t in T]
    parent = list(range(n))

    def vind(a):
        while parent[a] != a:
            parent[a] = parent[parent[a]]
            a = parent[a]
        return a

    def voeg(a, b):
        ra, rb = vind(a), vind(b)
        if ra != rb:
            parent[ra] = rb

    for i in range(n):
        if extreem[i]:
            for j in buren[i]:
                if extreem[j]:
                    voeg(i, j)
    from collections import Counter
    groottes = Counter(vind(i) for i in range(n) if extreem[i])
    gros = list(groottes.values())
    print("  extreme cellen: %d (%.1f%%); componenten>=2: %d; grootste: %d"
          % (sum(extreem), 100.0 * sum(extreem) / n,
             sum(1 for g in gros if g >= 2), max(gros) if gros else 0))

    # 4) windsnelheidsverdeling
    print("  |wind|        : p50=%.3f  p95=%.3f  max=%.3f (cap %.0f)"
          % (median_percentiel(W, 50), median_percentiel(W, 95), max(W), 2.0))
    return 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("gebruik: thermischProfiel.py <dump.csv>")
        sys.exit(2)
    sys.exit(main(sys.argv[1]))
