# ChessBot
A basic chess bot written in c++

Searching Algorithm: Negamax https://www.chessprogramming.org/Negamax

Searching Optimizations(For making Search Faster)
  1. Alpha Beta Pruning https://www.chessprogramming.org/Alpha-Beta
  2. Transposition Tables https://www.chessprogramming.org/Transposition_Table
  3. Iterative Deepening https://www.chessprogramming.org/Iterative_Deepening
  4. Null move pruning https://www.chessprogramming.org/Null_Move_Pruning 
  5. Killer move https://www.chessprogramming.org/Killer_Heuristic

To avoid the Horizon Effect(https://www.chessprogramming.org/Horizon_Effect), I've used depth-limited quienscience search. I've depth-limited Quiescence Search
as a compromise to save time. https://www.chessprogramming.org/Quiescence_Search

NOTE: This is a hobby project and currently it doesn't qualify as a competitive chess bot.
