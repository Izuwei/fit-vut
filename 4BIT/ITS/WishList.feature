Feature: Seznam přání

    Background:
        Given Uživatel je přihlášený do eshopu

    Scenario: Přidání položky do seznamu
        Given Je zobrazena detailní stránka produktu "Samsung Galaxy Tab 10.1"
        When Uživatel klikne na "Add to Wish List"
        Then Zobrazí se zpráva "Success: You have added Samsung Galaxy Tab 10.1 to your wish list!"

    Scenario: Zobrazení seznamu přání
        When Uživatel klikne na "Wish List"
        Then Zobrazí se seznam přání

    Scenario: Odebrání položky ze seznamu
        Given Pložka "Samsung Galaxy Tab 10.1" se nachází v seznamu
        And Je zobrazen seznam přání
        When Uživatel klikne na "Remove" pro Odebrání položky
        Then Zobrazí se zpráva "Success: You have modified your wish list!"

    Scenario: Návrat z prázdého sezamu přání
        Given Je zobrazený prázdný seznam přání
        When Uživatel klikne na "Continue"
        Then Zobrazí se stránka o účtu