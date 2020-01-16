Feature: Uživatel kontaktuje majitele

    Scenario: Zobrazení kontaktního formuláře
        Given Uživatel je na domovské stránce eshopu
        When Uživatel klikne na "Contact Us"
        Then Zobrazí se kontaktní formulář

    Scenario: Odeslání kontaktního formuláře
        Given Je zobrazen kontaktní formulář
        When Uživatel správně vyplní povinné údaje
        And Klikne na "Submit"
        Then Zobrazí se zpráva "Your enquiry has been successfully sent to the store owner!"