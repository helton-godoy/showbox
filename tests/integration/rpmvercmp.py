#!/usr/bin/env python3
"""Port fiel do rpmvercmp (RPM) para validar a ordenação dos releases RPM.

Espelha `rpmio/rpmvercmp.cc` do rpm-software-management/rpm (convenção de
saída do rpmdev-vercmp): compara duas strings de versão-release e sai com
0 (iguais), 11 (primeira mais nova) ou 12 (primeira mais antiga).

Uso: rpmvercmp.py <versão-a> <versão-b>
"""

import sys


def _segment_end(value: str, start: int, is_numeric: bool) -> int:
    end = start
    while end < len(value):
        ch = value[end]
        if is_numeric and not ch.isdigit():
            break
        if not is_numeric and not ch.isalpha():
            break
        end += 1
    return end


def rpmvercmp(a: str, b: str) -> int:
    if a == b:
        return 0
    one = two = 0
    n1, n2 = len(a), len(b)
    while one < n1 or two < n2:
        while one < n1 and not (a[one].isalnum() or a[one] in "~^"):
            one += 1
        while two < n2 and not (b[two].isalnum() or b[two] in "~^"):
            two += 1
        c1 = a[one] if one < n1 else ""
        c2 = b[two] if two < n2 else ""
        if c1 == "~" or c2 == "~":
            if c1 != "~":
                return 1
            if c2 != "~":
                return -1
            one += 1
            two += 1
            continue
        if c1 == "^" or c2 == "^":
            if c1 == "":
                return -1
            if c2 == "":
                return 1
            if c1 != "^":
                return 1
            if c2 != "^":
                return -1
            one += 1
            two += 1
            continue
        if c1 == "" or c2 == "":
            break
        s1 = one
        s2 = two
        is_numeric = c1.isdigit()
        e1 = _segment_end(a, s1, is_numeric)
        e2 = _segment_end(b, s2, is_numeric)
        if s1 == e1:
            return -1
        if s2 == e2:
            return 1 if is_numeric else -1
        seg1 = a[s1:e1]
        seg2 = b[s2:e2]
        if is_numeric:
            seg1 = seg1.lstrip("0") or "0"
            seg2 = seg2.lstrip("0") or "0"
            if len(seg1) > len(seg2):
                return 1
            if len(seg1) < len(seg2):
                return -1
        if seg1 > seg2:
            return 1
        if seg1 < seg2:
            return -1
        one = e1
        two = e2
    if not one < n1 and not two < n2:
        return 0
    if not one < n1:
        return -1
    return 1


def main(argv: list[str]) -> int:
    if len(argv) != 3:
        print(f"Uso: {argv[0]} <versão-a> <versão-b>", file=sys.stderr)
        return 2
    rc = rpmvercmp(argv[1], argv[2])
    if rc > 0:
        return 11
    if rc < 0:
        return 12
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))