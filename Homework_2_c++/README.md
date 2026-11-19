# ФизКат — консольный учёт парка

## Сборка и запуск
```bash
g++ -std=c++17 -Wall -Wextra -o fizkat main.cpp
./fizkat < input.txt
```
Команды читаются построчно со стандартного ввода, выход — `quit` или конец ввода.

## Схема наследования
```
InventoryItem (абстрактный)          EnergyConsumer (абстрактный)
├── Transport (абстрактный)                 │
│   ├── Bike                                │
│   ├── ElectricScooter  ───────────────────┤  множественное наследование
│   ├── ElectricBike     ───────────────────┤
│   └── Monowheel        ───────────────────┤  (креатив)
└── Thing (абстрактный)                     │
    ├── Helmet                              │
    └── Dock             ───────────────────┘
```
Ромба нет: `InventoryItem` и `EnergyConsumer` не имеют общего предка, поэтому `virtual`-наследование не нужно.

Полиморфизм: `list`, `energy`, `novice` вызывают `inventoryId()`, `name()`, `energyPerDay()`, `isNoviceFriendly()` через `InventoryItem*`, без `dynamic_cast`.

## Smart-указатели
- `unique_ptr<InventoryItem>` — `Fleet::Entry::item`: парк единолично владеет позициями.
- `shared_ptr<const ModelSpec>` — `Transport::spec_`: одна спецификация вида (scooter, ebike, …) на все единицы этого вида; хранится в `Fleet::specs_`.
- `weak_ptr` — `Fleet::focus_`, команды `focus` / `show_focus`. Указывает на «жетон» позиции (`Entry::token`), который живёт ровно столько же, сколько позиция в парке. После `remove` жетон уничтожается, и `show_focus` печатает `removed`.

## Исключения
- `FleetError : std::exception` — общий базис.
- `InspectionError` — отказ техосмотра (в имени транспорта есть `broken` без учёта регистра).
- `ArgumentError` — неизвестная команда/тип, неверное число аргументов, простота вне 1..10, отрицательная или нечисловая энергия, пустой или повторный id, `remove`/`focus` несуществующего id.

Бросаются внутри `Fleet` и функций разбора, `error: ...` печатается только в `catch` в `main`. Объект добавляется в парк последним шагом, после всех проверок, поэтому ошибка не портит состояние.

## Операторы
- `operator<<(ostream&, const InventoryItem&)` — печать `id имя`.
- `operator<(const InventoryItem&, const InventoryItem&)` — сортировка по id в `list`.
- `operator<<` для `Registry`.

## Шаблоны
- `template<typename T, int Cap> class Registry` — журнал последних `Cap` записей (`add`, `size`, `operator[]`, `operator<<`). Используется как журнал техосмотров `Fleet::inspectionLog_`, команда `log`.
- `template<typename Container> double totalEnergy(const Container&)` — сумма энергии по контейнеру указателей, используется в `energy` и `report`.

## Креативная часть
- Новый вид транспорта `Monowheel`: `add monowheel id name energy score`.
- `max_energy` — позиция с наибольшим суточным расходом.
- `report` — число единиц и суммарная энергия.
- `log` — журнал принятых на техосмотре.

## Использование ИИ
read_me написано с помощью ии. также спрашивал всё, что не знаю сам, как оно работает, а затем вручную писал соответствующий код для задачи, спрашивая, где есть ошибки.
