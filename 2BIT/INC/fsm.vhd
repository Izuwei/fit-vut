-- fsm.vhd: Finite State Machine
-- Author(s): Jakub Sadilek
-- Login: xsadil07
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (TEST1, TEST2, TEST3, TEST4, TEST5, TEST6, TEST7, TESTA1, TESTA2, TESTB1, TESTB2, TESTB3, DONE, MESSAGE_SUCCESS, MESSAGE_FAILURE, WRONG_KEY, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- --------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- Test 1. znaku, ktery je spolecny pro oba kody. Pokud znak odpovida pokracujem. Pokud # odepreme pristup, jinak WRONG_KEY.
   when TEST1 =>
      next_state <= TEST1;
      if (KEY(8) = '1') then
         next_state <= TEST2;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- Test 2. znaku (spolecny).
   when TEST2 =>
      next_state <= TEST2;
      if (KEY(7) = '1') then
         next_state <= TEST3;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- Test 3. znaku (spolecny).
   when TEST3 =>
      next_state <= TEST3;
      if (KEY(1) = '1') then
         next_state <= TEST4;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- Test 4. znaku (spolecny).
   when TEST4 =>
      next_state <= TEST4;
      if (KEY(6) = '1') then
         next_state <= TEST5;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- Test 5. znaku (spolecny).
   when TEST5 =>
      next_state <= TEST5;
      if (KEY(5) = '1') then
         next_state <= TEST6;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- Test 6. znaku (spolecny).
   when TEST6 =>
      next_state <= TEST6;
      if (KEY(6) = '1') then
         next_state <= TEST7;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- Test 7. znaku (spolecny). Podle stisknuteho dalsiho tlacitka se vybere cesta (zadavaneho kodu).
   when TEST7 =>
      next_state <= TEST7;
      if (KEY(6) = '1') then
         next_state <= TESTA1;
		elsif (KEY(2) = '1') then
			next_state <= TESTB1;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- 1. kod: 8. znak.
   when TESTA1 =>
      next_state <= TESTA1;
      if (KEY(7) = '1') then
         next_state <= TESTA2;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- 2. kod: 8. znak.
   when TESTB1 =>
      next_state <= TESTB1;
		if (KEY(8) = '1') then
			next_state <= TESTB2;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- 1. kod: 9. znak.
   when TESTA2 =>
      next_state <= TESTA2;
      if (KEY(8) = '1') then
         next_state <= DONE;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- 2. kod: 9. znak.
   when TESTB2 =>
      next_state <= TESTB2;
      if (KEY(8) = '1') then
         next_state <= TESTB3;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- 2. kod: 10. znak.
   when TESTB3 =>
      next_state <= TESTB3;
      if (KEY(6) = '1') then
         next_state <= DONE;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- Pokud byl zadan spatny znak cyklime dokud #, potom odepreme pristup.
	when WRONG_KEY =>
		next_state <= WRONG_KEY;
		if (KEY(15) = '1') then
			next_state <= MESSAGE_FAILURE;
		end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- Pokud byl zadan spravny kod testujeme #, potom povolime pristup, jinak WRONG_KEY.
	when DONE =>
		next_state <= DONE;
		if (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG_KEY;
		elsif (KEY(15) = '1') then
			next_state <= MESSAGE_SUCCESS;
		end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- Vypis povoleneho pristupu.
   when MESSAGE_SUCCESS =>
      next_state <= MESSAGE_SUCCESS;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- Vypis odepreneho pristupu.
	when MESSAGE_FAILURE =>
		next_state <= MESSAGE_FAILURE;
		if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- Konec, pokud je opet zadana #, jdeme od zacatku.
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- Pro ostatni jdeme na zacatek (osetreni).
   when others =>
      next_state <= TEST1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- Vypis pristup povolen.
   when MESSAGE_SUCCESS =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
		FSM_MX_MEM		<= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- Vypis pristup odepren.
	when MESSAGE_FAILURE =>
		FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- U vypisu reagujeme pouze na #, ktere vycisti display.
	when FINISH =>
		if (KEY(15) = '1') then
			FSM_LCD_CLR	<= '1';
		end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- Jinak pro klavesy 0-14 * a pro # vycistime display.
   when others =>
		if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      elsif (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;
