Feature: Proces přihlášení uživatele

    Background:
        Given Uživatel není přihlášený

    Scenario: Zobrazení submenu účtu
        Given Uživatel je na domovské stránce eshopu
        When Uživatel klikne na "My Account"
        Then Zobrazí se submenu "My Account"
    
    Scenario: Zobrazení přihlašovacího formuláře
        Given Je zobrazené submenu "My Account"
        When Uživatel klikne na "Login"
        Then Zobrazí se přihlašovací formulář

    Scenario: Přihlášení
        Given Je zobrazen přihlašovací formulář
        When Uživatel správně vyplní email a heslo
        And Uživatel klikne na tlačítko "Login"
        Then Uživatel je přesměrován na stránku účtu