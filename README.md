# HotCoffeeBot
Semi-useless robot for ASK

## TODO:
- [X] Funkcja mierząca siłe/pozycję źródła
- [ ] zamontować ekstra czujniki 
- [ ] zmodylikować funkcje do nie wjebywania się na ściany
- [ ] ogarnać schemat w eaglu
- [ ] ogarnąć schemat do programowania arduino

## Looking for help 
- trzeba ulepszyć funkcje i kod do obliczania odległośći za pomocą rssi, narazie moim pomysłem jest zrobienie kilku pomiarów za pomocą anten (rssi/odległość) robota i wrzucić do octave-a żeby znarazl krzywe najlepszego dopasowania, ale inne propozycje też mile widziane (nie chazan zmontowanie filniku tak żeby działało to nie rozwiązanie).
- AI robota aka. zczytanie odległości z czujników i wifi i podojmowanie decyzji na ich podstawie (tak porządnie).
- ogarnianie podłączenia atmegi w celu jej zaprogramowania.

## Pending:
- [X] zrobie schemat obecnych połączeń na arduino bo sam się już w nich gubię.(niestety nie było płytek esp w eaglu więc macie same połączenia na arduino i mostek h)
- [ ] wrzuce w osobnym pliku .md obecny wzór na odległość i lepiej opiszę.
- [ ] dorobie przedni zderzak na ekstra sensory.
- [ ] przerzuce tu (albo na dysk) zdjęcia robota i odległości bo troche się na nim pozmieniało.
- [ ] zrobie płytkę pod usbasp jeszcze raz i zobacze czy coś się uda zrobić jeszcze z tamtą atmegą.
- [ ] opiszę funkcje związane z wifi i je uporządkuje.

## Propozycje: 
- Przerobić funkcję sonaru tak żeby brała średnią z 10 próbek i ignorowała niezgodne odczyty (lepsza wiarygodość)

    Ogólnie piszcie albo DM albo na grupie jak będziecie chcieli pomóc i mieli z czymś problem.
---
## ~~SEKCJA CIERPIENIA UART~~ zrobione 
---
## Obecny wzór na odległość - trzeba wprowadzić  do niego parametry i eksperymentalnie wyprowadzić
## 

$Db = 10* \log_{10}(W)$

$Db = 10 *\log_{10}(\frac{V}{r^2})$

$Db_{r0} - Db_{r1} = 10 * log_{10}(\frac{V}{r_0^2})- 10*log_{10}(\frac{V}{r_1^2}) $

$\frac{\Delta Db}{10} = 2*log_{10}{\frac{r_1}{r_0}}$

$10^\frac{\Delta Db}{20} = \frac{r1}{r0}$

$r_1 = r_0*10^\frac{\Delta Db}{20} $


