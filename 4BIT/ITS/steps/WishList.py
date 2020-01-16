import selenium
from behave import *
from time import sleep

@given('Uživatel je přihlášený do eshopu')
def step(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=account/account')
    sleep(1)
    if (context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=account/login'):
        context.browser.find_element_by_id('input-email').send_keys('test@test.cz')
        context.browser.find_element_by_id('input-password').send_keys('testtest123')
        context.browser.find_element_by_xpath('//input[@type="submit"][@value="Login"]').click()
        sleep(1)

@given('Je zobrazena detailní stránka produktu "Samsung Galaxy Tab 10.1"')
def step(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=product/product&path=57&product_id=49')

@when('Uživatel klikne na "Add to Wish List"')
def step(context):
    context.browser.find_element_by_xpath('//button[@data-original-title="Add to Wish List"]').click()
    sleep(1)

@then('Zobrazí se zpráva "Success: You have added Samsung Galaxy Tab 10.1 to your wish list!"')
def step(context):
    text = context.browser.find_element_by_css_selector('div.alert.alert-success').text[:-2]
    assert('Success: You have added Samsung Galaxy Tab 10.1 to your wish list!' == text)

@when('Uživatel klikne na "Wish List"')
def step(context):
    context.browser.find_element_by_id('wishlist-total').click()
    sleep(1)

@then('Zobrazí se seznam přání')
def step(context):
    assert(context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=account/wishlist')

@given('Pložka "Samsung Galaxy Tab 10.1" se nachází v seznamu')
def step(context):
    prevURL = context.browser.current_url
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=product/product&product_id=49')
    sleep(1)
    context.browser.find_element_by_id('wishlist-total').click()
    context.browser.get(prevURL)
    sleep(1)

@given('Je zobrazen seznam přání')
def step(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=account/wishlist')
    sleep(1)

@when('Uživatel klikne na "Remove" pro odebrání položky')
def step(context):
    context.browser.find_element_by_xpath('//a[@data-original-title="Remove"]').click()
    sleep(1)

@then('Zobrazí se zpráva "Success: You have modified your wish list!"')
def step(context):
    text = context.browser.find_element_by_css_selector('div.alert.alert-success').text[:-2]
    assert('Success: You have modified your wish list!' == text)

@given('Je zobrazený prázdný seznam přání')
def step(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=account/wishlist')
    sleep(1)
    assert('Your wish list is empty.' == context.browser.find_element_by_xpath('//div[@id="content"]/p').text)

@then('Zobrazí se stránka o účtu')
def step(context):
    assert(context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=account/account')
