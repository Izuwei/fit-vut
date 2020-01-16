Feature: Operace s produkty v kosiku

    Scenario: Vložení produktu do košíku
        Given Uživatel se nachází na detailní stránce produktu "iPhone"
        When Uživatel klikne na "Add to Cart"
        Then "iPhone" je přidán do košíku

    Scenario: Zobrazení košíku
        Given Košík obsahuje "iPhone"
        When Uživatel klikne na "Shopping Cart"
        Then Zobrazí se košík

    Scenario: Změna množství
        Given Uživatel se nachází v košíku
        And Košík obsahuje "iPhone"
        When Uživatel změní množství na jiné přirozené číslo
        And Uživatel klikne na "Update"
        Then Změní se počet položek "iPhone" v košíku

    Scenario: Změna na záporné množství produktů
        Given Uživatel se nachází v košíku
        And Košík obsahuje "iPhone"
        When Uživatel změní množství na záporné číslo
        And Uživatel klikne na "Update"
        Then Zobrazí se zpráva "Your shopping cart is empty!"

    Scenario: Odebrání položky z košíku
        Given Uživatel se nachází v košíku
        And Košík obsahuje "iPhone"
        When Uživatel klikne na "Remove"
        Then Zobrazí se zpráva "Your shopping cart is empty!"

    Scenario: Přesměrování z prázdného košíku na domovskou stránku eshopu
        Given Uživatel se nachází v košíku
        And Košík je prázdný
        When Uživatel klikne na "Continue"
        Then Uživatel je přesměrován na domovskou stránku eshopu