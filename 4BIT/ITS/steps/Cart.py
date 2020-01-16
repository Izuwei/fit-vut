import selenium
from behave import *
from time import sleep

@given('Uživatel se nachází na detailní stránce produktu "iPhone"')
def step(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=product/product&path=24&product_id=40')

@when('Uživatel klikne na "Add to Cart"')
def step(context):
    context.browser.find_element_by_id('button-cart').click()
    sleep(1)

@then('"iPhone" je přidán do košíku')
def step(context):
    text = context.browser.find_element_by_css_selector('div.alert.alert-success').text[:-2]
    assert('Success: You have added iPhone to your shopping cart!' == text)

@when('Uživatel klikne na "Shopping Cart"')
def step(context):
    context.browser.find_element_by_xpath("//div[@id='top-links']/ul/li[4]/a/span").click()

@then('Zobrazí se košík')
def step(context):
    assert(context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=checkout/cart')

@given('Uživatel se nachází v košíku')
def step(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=checkout/cart')

@given('Košík obsahuje "iPhone"')
def step(context):
    prevURL = context.browser.current_url
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=product/product&path=24&product_id=40')
    sleep(1)
    context.browser.find_element_by_id('button-cart').click()
    context.browser.get(prevURL)
    sleep(1)

@when('Uživatel změní množství na jiné přirozené číslo')
def step(context):
    context.browser.find_element_by_xpath('//input[starts-with(@name, "quantity")]').clear()
    context.browser.find_element_by_xpath('//input[starts-with(@name, "quantity")]').send_keys('2')
    
@when('Uživatel klikne na "Update"')
def step(context):
    context.browser.find_element_by_css_selector('button.btn.btn-primary').click()
    sleep(1)

@then('Změní se počet položek "iPhone" v košíku')
def step(context):
    value = context.browser.find_element_by_xpath('//input[starts-with(@name, "quantity")]').get_attribute('value')
    assert(value == '2')

@when('Uživatel změní množství na záporné číslo')
def step(context):
    context.browser.find_element_by_xpath('//input[starts-with(@name, "quantity")]').clear()
    context.browser.find_element_by_xpath('//input[starts-with(@name, "quantity")]').send_keys('-1')

@then('Zobrazí se zpráva "Your shopping cart is empty!"')
def step(context):
    text = context.browser.find_element_by_xpath('//div[@id="content"]/p').text
    assert(text == 'Your shopping cart is empty!')

@when('Uživatel klikne na "Remove"')
def step(context):
    context.browser.find_element_by_xpath('//button[@data-original-title="Remove"]').click()
    sleep(1)

@given('Košík je prázdný')
def step(context):
    text = context.browser.find_element_by_xpath('//div[@id="content"]/p').text
    assert(text == 'Your shopping cart is empty!')

@when('Uživatel klikne na "Continue"')
def step(context):
    context.browser.find_element_by_link_text('Continue').click()
    sleep(1)

@then('Uživatel je přesměrován na domovskou stránku eshopu')
def step(context):
    assert(context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=common/home')
