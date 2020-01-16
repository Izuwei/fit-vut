import selenium
from behave import *
from time import sleep

@given('Uživatel je na domovské stránce eshopu')
def step(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8054')

@when('Uživatel klikne na položku "Cameras"')
def step(context):
    context.browser.find_element_by_link_text('Cameras').click()

@then('Zobrazí se nabízené položky v "Cameras"')
def step(context):
    assert(context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=product/category&path=33')

@given('Jsou zobrazené položky "Cameras"')
def step(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=product/category&path=33')

@when('Uživatel klikne na obrázek položky "Nikon D300"')
def step(context):
    context.browser.find_element_by_xpath('//a[@href="http://mys01.fit.vutbr.cz:8054/index.php?route=product/product&path=33&product_id=31"]').click()

@then('Zobrazí se podrobnosti položky "Nikon D300"')
def step(context):
    assert(context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=product/product&path=33&product_id=31')

@given('Jsou zobrazené podrobnosti položky "Nikon D300"')
def step(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=product/product&path=33&product_id=31')

@then('Položka je vložena do košíku')
def step(context):
    text = context.browser.find_element_by_css_selector('div.alert.alert-success').text[:-2]
    assert(u'Success: You have added Nikon D300 to your shopping cart!' == text)

@given('Položka je vložena v košíku')
def step(context):
    prevURL = context.browser.current_url
    context.browser.get('http://mys01.fit.vutbr.cz:8054/index.php?route=product/product&path=33&product_id=31')
    sleep(1)
    context.browser.find_element_by_id('button-cart').click()
    context.browser.get(prevURL)
    sleep(1)

@when('Uživatel klikne na "Checkout"')
def step(context):
    context.browser.find_element_by_link_text('Checkout').click()
    sleep(1)

@then('Zobrazí se formulář s fakturačními údaji')
def step(context):
    assert(context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=checkout/checkout')

@when('Uživatel vybere možnost "Guest Checkout"')
def step(context):
    context.browser.find_element_by_xpath('//input[@name="account"][@value="guest"]').click()

@when('Uživatel zmáčkne na "Continue" v prvním kroku')
def step(context):
    context.browser.find_element_by_id('button-account').click()
    sleep(1)

@then('Zobrazí se druhý krok fakturace')
def step(context):
    assert('true' == context.browser.find_element_by_xpath('//a[@href="#collapse-payment-address"]').get_attribute('aria-expanded'))

@when('Uživatel správně vyplní všechny povinné údaje v druhém kroku')
def step(context):
    context.browser.find_element_by_id('input-payment-firstname').send_keys('test')
    context.browser.find_element_by_id('input-payment-lastname').send_keys('test')
    context.browser.find_element_by_id('input-payment-email').send_keys('test@test.cz')
    context.browser.find_element_by_id('input-payment-telephone').send_keys('123456789')
    context.browser.find_element_by_id('input-payment-address-1').send_keys('test')
    context.browser.find_element_by_id('input-payment-city').send_keys('test')
    context.browser.find_element_by_id('input-payment-postcode').send_keys('12345')
    context.browser.find_element_by_xpath("(//option[@value='3513'])").click()

@when('Uživatel potvrdí shodu fakturační a doručovací adresy')
def step(context):
    if ((context.browser.find_element_by_xpath('//input[@type="checkbox"][@name="shipping_address"]').get_attribute('value')) == '0'):
        context.browser.find_element_by_xpath('//input[@type="checkbox"][@name="shipping_address"]').click()

@when('Uživatel klikne na "Continue" v druhém kroku')
def step(context):
    context.browser.find_element_by_id('button-guest').click()
    sleep(1)

@then('Zobrazí se čtvrtý krok fakturace')
def step(context):
    assert('true' == context.browser.find_element_by_id('collapse-shipping-method').get_attribute('aria-expanded'))

@when('Uživatel klikne na "Continue" u zpusobu dodani')
def step(context):
    context.browser.find_element_by_id('button-shipping-method').click()
    sleep(1)

@then('Zobrazí se pátý krok fakturace')
def step(context):
    assert('true' == context.browser.find_element_by_id('collapse-payment-method').get_attribute('aria-expanded'))

@when('Uživatel souhlasí se smluvními podmínkami')
def step(context):
    context.browser.find_element_by_xpath('//input[@type="checkbox"][@name="agree"]').click()

@when('Uživatel klikne na "Continue" u způsobu platby')
def step(context):
    context.browser.find_element_by_id('button-payment-method').click()
    sleep(1)

@then('Zobrazí se šestý krok fakturace')
def step(context):
    assert('true' == context.browser.find_element_by_id('collapse-checkout-confirm').get_attribute('aria-expanded'))

@when('Uživatel klikne na "Confirm Order"')
def step(context):
    context.browser.find_element_by_id('button-confirm').click()
    sleep(1)

@then('Objednávka je uspěšně dokončena')
def step(context):
    assert(context.browser.current_url == 'http://mys01.fit.vutbr.cz:8054/index.php?route=checkout/success')
