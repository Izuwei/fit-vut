Feature: Proces nákupu kamery
    
    Scenario: Vyhledání kamery
        Given Uživatel je na domovské stránce eshopu
        And Uživatel není přihlášený
        When Uživatel klikne na položku "Cameras"
        Then Zobrazí se nabízené položky v "Cameras"

    Scenario: Zobrazení podrobnosti kamery
        Given Jsou zobrazené položky "Cameras"
        When Uživatel klikne na obrázek položky "Nikon D300"
        Then Zobrazí se podrobnosti položky "Nikon D300"

    Scenario: Přidání kamery do košíku
        Given Jsou zobrazené podrobnosti položky "Nikon D300"
        When Uživatel klikne na "Add to Cart"
        Then Položka je vložena do košíku

    Scenario: Zobrazení košíku
        Given Položka je vložena v košíku
        When Uživatel klikne na "Shopping Cart"
        Then Zobrazí se košík

    Scenario: Zobrazení fakturačních údajů
        Given Uživatel se nachází v košíku
        And Položka je vložena v košíku
        When Uživatel klikne na "Checkout"
        Then Zobrazí se formulář s fakturačními údaji

    Scenario: První krok fakturace - možnosti fakturace
        When Uživatel vybere možnost "Guest Checkout"
        And Uživatel zmáčkne na "Continue" v prvním kroku
        Then Zobrazí se druhý krok fakturace

    Scenario: Druhý krok fakturace - podrobnosti fakturace
        When Uživatel správně vyplní všechny povinné údaje v druhém kroku
        And Uživatel potvrdí shodu fakturační a doručovací adresy
        And Uživatel klikne na "Continue" v druhém kroku
        Then Zobrazí se čtvrtý krok fakturace

    Scenario: Čtvrtý krok fakturace - způsob dodání
        When Uživatel klikne na "Continue" u zpusobu dodani
        Then Zobrazí se pátý krok fakturace

    Scenario: Pátý krok fakturace - způsob platby
        When Uživatel souhlasí se smluvními podmínkami
        And Uživatel klikne na "Continue" u způsobu platby
        Then Zobrazí se šestý krok fakturace

    Scenario: Šestý krok fakturace - potvrzení objednávky
        When Uživatel klikne na "Confirm Order"
        Then Objednávka je uspěšně dokončena