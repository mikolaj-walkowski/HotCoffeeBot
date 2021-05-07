# HotCoffeeBot
Semi-useless robot for ASK

## TODO:
- [ ] Funkcja mierząca siłe/pozycję źródła
- [ ] zamontować ekstra czujnik 
- [ ] zmodylikować funkcje do nie wjebywania się na ściany
- [ ] ogarnać schemat w eaglu 

## Propozycje: 
- Przerobić funkcję sonaru tak żeby brała średnią z 10 próbek i ignorowała niezgodne odczyty (lepsza wiarygodość)

---
## SEKCJA CIERPIENIA UART
Problem z wifi jest łatwy do rozwiązania wystarczy zmienić szybkość Software serial z 115200 na 9600 i elo wyniki są czyste, ale trzeba to zrobić także na modułach wifi.
- Komenda AT+IPR=9600 nie działa  i rozjebała software płytek.
- Trzeba zrobić flusha obu płytek i ZAPROGRAMOWAĆ je na 9600 ew istneije też komenda AT+UART_DEF=9600,8,1,0,0 ona ponoć nie bricuje software-u
- Re-flushowanie ESP8266 jest zagadnieniem pełnym magi i czarów będe wstawiał linki dalej z tego co zrobiłem i schematy okablowania jakby któryś z was miał pomysły.
