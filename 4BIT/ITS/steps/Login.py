import selenium
from behave import *

@given('Uživatel není přihlášený')
def step(context):
    prevURL = context.browser.current_url
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=account/login')
    if (context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=account/account'):
        context.browser.find_element_by_xpath('//a[@href="http://mys01.fit.vutbr.cz:8054/index.php?route=account/account"]').click()
        context.browser.find_element_by_xpath('//a[@href="http://mys01.fit.vutbr.cz:8054/index.php?route=account/logout"]').click()
    context.browser.get(prevURL)

@when('Uživatel klikne na "My Account"')
def step(context):
    context.browser.find_element_by_xpath('//a[@title="My Account"][@data-toggle="dropdown"]').click()

@then('Zobrazí se submenu "My Account"')
def step(context):
    assert('true' == context.browser.find_element_by_xpath('//a[@href="http://mys01.fit.vutbr.cz:8054/index.php?route=account/account"]').get_attribute('aria-expanded'))

@given('Je zobrazené submenu "My Account"')
def step(context):
    context.browser.find_element_by_xpath('//a[@title="My Account"][@data-toggle="dropdown"]').click()

@when('Uživatel klikne na "Login"')
def step(context):
    context.browser.find_element_by_xpath('//a[@href="http://mys01.fit.vutbr.cz:8054/index.php?route=account/login"]').click()

@then('Zobrazí se přihlašovací formulář')
def step(context):
    assert(context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=account/login')

@given('Je zobrazen přihlašovací formulář')
def step(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=account/login')

@when('Uživatel správně vyplní email a heslo')
def step(context):
    context.browser.find_element_by_id('input-email').send_keys('test@test.cz')
    context.browser.find_element_by_id('input-password').send_keys('testtest123')

@when('Uživatel klikne na tlačítko "Login"')
def step(context):
    context.browser.find_element_by_xpath('//input[@type="submit"][@value="Login"]').click()

@then('Uživatel je přesměrován na stránku účtu')
def step(context):
    assert(context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=account/account')
