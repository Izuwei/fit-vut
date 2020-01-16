import selenium
from behave import *

@when('Uživatel klikne na "Contact Us"')
def step(context):
    context.browser.find_element_by_xpath('//a[@href="http://mys01.fit.vutbr.cz:8054/index.php?route=information/contact"]').click()

@then('Zobrazí se kontaktní formulář')
def step(context):
    assert(context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=information/contact')

@given('Je zobrazen kontaktní formulář')
def step(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=information/contact')

@when('Uživatel správně vyplní povinné údaje')
def step(context):
    context.browser.find_element_by_id('input-name').send_keys('testttestest')
    context.browser.find_element_by_id('input-email').send_keys('tetestst@test.cz')
    context.browser.find_element_by_id('input-enquiry').send_keys('Defitesttesttestnitly not spam.')

@when('Klikne na "Submit"')
def step(context):
    context.browser.find_element_by_xpath('//input[@type="submit"][@value="Submit"]').click()

@then('Zobrazí se zpráva "Your enquiry has been successfully sent to the store owner!"')
def step(context):
    assert(context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=information/contact')
