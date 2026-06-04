# Code Review — Chess Engine

The overall structure is genuinely solid for a learning project. The piece hierarchy, negamax with alpha-beta, Zobrist hashing and a transposition table all show real ambition and mostly correct understanding. Below are the bugs, roughly ordered from most to least severe.

---

## Bug 1 — Bishop causes an infinite loop / out-of-bounds crash (strelec.cpp:59)

This is the most dangerous bug in the project. The bishop's `validniTah` never checks that the move is actually diagonal (`abs(dx) == abs(dy)`). The function computes `smerX` and `smerY` assuming the move is diagonal, then marches the ray diagonally. For a non-diagonal, non-axis-aligned target — e.g. bishop at `(2,2)` to `(0,3)` — `smerX=1, smerY=-1`, and the loop marches `(3,1), (4,0), (5,-1)…` and reads `pozice[y][x]` completely out of bounds.

The game only "works" because that out-of-bounds read happens to return non-NULL garbage, which makes `jeFigurka` return true, which exits the loop with `return false`. It's surviving on undefined behavior. Fix:

```cpp
bool Strelec::validniTah(int fromY, int fromX, int toY, int toX, Sachovnice *s)
{
    int dx = toX - fromX;
    int dy = toY - fromY;
    if (abs(dx) != abs(dy) || dx == 0) return false;  // add this line
    ...
```

---

## Bug 2 — Stalemate is treated as a loss (sachovnice.cpp:254)

When `negaMax` runs out of legal moves (the inner loops finish without any valid move) `maxEval` stays at `-99999` and gets stored. But "no legal moves + not in check" is stalemate — a draw, which should score `0`. Currently the engine treats stalemate identically to checkmate. In practice this means it may sacrifice winning positions to force a stalemate "win" that isn't actually a win, or conversely avoid positions that would be forced stalemates.

Fix — after the four nested loops, before storing to the table:

```cpp
if (maxEval == -99999) {
    // no legal move found
    maxEval = jeSach(barva) ? -99999 : 0;  // checkmate vs stalemate
}
```

---

## Bug 3 — `reset()` leaves `tahZpet` intact with dangling pointers (sachovnice.cpp:114)

`reset()` calls `delete` on every piece currently on the board. But `tahZpet` (the undo stack) holds `Tah::vyhozena` pointers to pieces that were captured during the game — pieces that are no longer in `pozice` and are therefore never deleted by `reset()`. Two consequences:

1. **Memory leak** — captured pieces are leaked on every reset.
2. **Use-after-free** — after a reset, `tahniZpetuser()` can still be called, which pops from `tahZpet` and tries to restore `posledni.vyhozena` back onto the board. Those pointers now point to freed memory.

Fix: at the end of `reset()`:

```cpp
// free any captured pieces still in the undo stack
while (!tahZpet.empty()) {
    Tah& t = tahZpet.back();
    if (t.vyhozena) delete t.vyhozena;
    tahZpet.pop_back();
}
```

---

## Bug 4 — Button callback is never invoked (menu.cpp:51)

```cpp
void Tlacitko::klik() {
    funkce;   // just names the variable, does nothing
}
```

`funkce` is a `std::function<void()>`. Writing its name without `()` is a no-op. Every button in the game silently does nothing when clicked through the Menu system. The buttons in `main.cpp` happen to work only because their click logic is wired by hand with raw pixel comparisons, completely bypassing `Menu::klik()`.

Fix: `funkce();`

---

## Bug 5 — Memory leak: a new button is allocated every single frame (main.cpp:43)

```cpp
while(1) {
    sprintf(hodnota, "%d", s.hodnota);
    Body.pridej(new Tlacitko(hodnota, [&](){}));  // called ~60 times per second
```

A brand new `Tlacitko` is `new`-ed and added to the list on every frame. They are never deleted. After a few minutes the list has tens of thousands of heap-allocated buttons, all drawn on top of each other at the same coordinates, leaking memory continuously.

The `Ukazatel` class already exists and is designed exactly for this purpose. Either move the `Body` setup outside the loop and update its label each frame, or just use `Ukazatel` which takes a reference to an `int` and renders it live.

---

## Bug 6 — Castling allows moving through check (kral.cpp:91)

Castling validation correctly checks that the squares between king and rook are empty. It does **not** check:

1. The king is currently in check (castling out of check is illegal).
2. The king passes through a threatened square (castling through check is illegal).

`validniTahSach` will verify the king's **destination** is safe (because it temporarily moves the king there and calls `jeSach`), but the intermediate square is never verified. You can legally castle while in check or through an attacked square.

Fix — inside `Kral::validniTah`, before returning `true` for castling:

```cpp
// kingside: king passes through column 5
if (toX == 6) {
    if (s->jeSach(barva)) return false;  // can't castle while in check
    if (s->jePolickoOhrozeno(5, fromY, (barva == BILAF) ? CERNAF : BILAF)) return false;
}
// queenside: passes through columns 3 and 2
if (toX == 2) {
    if (s->jeSach(barva)) return false;
    if (s->jePolickoOhrozeno(3, fromY, (barva == BILAF) ? CERNAF : BILAF)) return false;
}
```

---

## Bug 7 — Transposition table uses only 6% of its allocated memory (sachovnice.cpp:242)

```cpp
pole = new Hashtable[1 << 25];  // allocates 33,554,432 entries ≈ 32 MB
...
int tableIndex = hash % 2000000;  // but only 2,000,000 slots are ever touched
```

The array is 16× bigger than the modulus. Either shrink the allocation to match:

```cpp
pole = new Hashtable[2000000];
```

Or use the full table (which actually reduces collisions and improves AI strength):

```cpp
const int TABLE_SIZE = 1 << 23;  // must be power-of-2 for the & trick
pole = new Hashtable[TABLE_SIZE];
...
int tableIndex = hash & (TABLE_SIZE - 1);  // faster than %, works for power-of-2
```

---

## Bug 8 — `prank()` is completely broken (sachovnice.cpp:90) — dead code

Three mistakes in one function: the column loop goes `c = 0..1` so `c == 4` is never true; `new Kral(...)` and `new Kralovna(...)` are allocated but the returned pointer is discarded (memory leak); and the function is commented out in `main.cpp` anyway. This is dead code and should be removed.

---

## Minor issues worth knowing

- **`Figurka::hodnotaFigurky`** (`figurka.cpp:13`) — the base class implementation has an empty body with no `return`. If it is ever called (it shouldn't be since all pieces override it), that is undefined behavior.
- **Knight positional table** (`kun.cpp:22`) — the knight uses one table for both colors. All other pieces have mirrored white/black tables. The knight's table happens to be symmetric so the result is the same, but it's inconsistent.
- **`goto` in `jeSach`** (`sachovnice.cpp:513`) — not a bug, but a labeled `goto` can be replaced with a helper function or a `bool found` flag for cleaner code.
- **`Strelec::validniTah` dead code** (`strelec.cpp:80`): `if (!s->jeNepritel(...)) return false;` is unreachable — the preceding `if (s->jeNepritel(...)) return true;` already handles the only remaining case.

---

## What to work on before summer break

Ordered from "most impactful fix" to "most interesting new feature":

1. **Fix bugs 1–3 first** (bishop UB, stalemate, reset/dangling pointers). These affect correctness on every game.
2. **Fix bug 6** (castling through check). It's one chess rule that experienced players will immediately spot.
3. **En passant** — the one missing pawn rule. The `Tah` struct would need a flag for it, similar to how `rosada` and `promoce` are already tracked. A good exercise because it requires modifying the undo mechanism too.
4. **Promotion choice** — currently a pawn always promotes to a queen. Adding a simple dialog (even just four buttons) makes the game feel complete.
5. **Move the AI to a background thread** — right now `robot()` blocks the event loop for the entire search (depth 5 can take a second or more in complex positions). The SDL window freezes. `pthread` is already included in `main.cpp` — this is a natural place to use it.
6. **Iterative deepening** — instead of always searching to depth 5, search depth 1, then 2, then 3, etc., stopping when time runs out. This always gives *some* answer quickly and usually reaches the same depth-5 result. It also enables move ordering (try the best move from the previous iteration first), which dramatically improves alpha-beta pruning efficiency.
