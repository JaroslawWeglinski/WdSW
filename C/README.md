# C. Opóźnienia programowe i sprzętowe

## C.1. Opóźnienie programowe

[Kod do zadania C.1.](./c1.c)

### Ile cykli potrzeba do wykonania 500 ms opóźnienia?

16 MHz / 16 = 1 MHz = 0.001ms (okres)
500 ms / 0.001ms = 500000

![filmik c1](./c1.gif)

Okres mrugania diody nie różnił się zauważalnie od 500ms.

## C.2. Opóźnienie programowe – wersja ROM

[Kod do zadania C.2.](./c2.c)

![filmik c2](./c2.gif)

## C.3. Licznik czasu

[Kod do zadania C.3.](./c3.c)

### Maksymalna wartość, która zmieści się w połowie licznika

`2^16 - 1 = 65535`

### Maksymalna wartość, która zmieści się w całym liczniku?

`2^32 - 1 = 4 294 967 295`

### Liczba potrzebna do zmieszczenia wszystkich cykli?

`625000`

### Który typ licznika (połowiczny czy pełny) pomieści potrzebną liczbę cykli?

Wyłącznie pełny licznik jest w stanie pomieścić potrzebną liczbę cykli.

### Dlaczego ładujemy do timera liczbę cykli pomniejszoną o jeden?

Ponieważ timer resetuje się przy przejściu przez zero, a nie na samej wartości zera.

### Jaki jest maksymalny czas, który może zmierzyć licznikiem pełnej długości?

4,294,967,295 * 0.001ms

![filmik c3](./c3.gif)
