library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-- Jmeno: Jakub Sadílek
-- Login: xsadil07

entity ledc8x8 is
port ( -- Rozhrani obvodu.
	SMCLK : in std_logic;
	RESET : in std_logic;
	ROW : out std_logic_vector(7 downto 0);
	LED : out std_logic_vector(7 downto 0)
);
end ledc8x8;

architecture main of ledc8x8 is

-- Definice vnitrnich signalu.
	 signal counter : std_logic_vector(11 downto 0) := (others => '0'); -- 1110 0001 0000 = (SMCLK/256/8) pocitadlo
	 signal rows : std_logic_vector(7 downto 0);	-- Radky
	 signal cols : std_logic_vector(7 downto 0);	-- Sloupce (ledky)
	 signal clk_enable : std_logic := '0';	-- Enable signal
	 signal state : std_logic_vector(1 downto 0) := "00";	-- Stavy
	 signal swap_counter : std_logic_vector(20 downto 0) := (others => '0'); -- 1 1100 0010 0000 0000 0000 = (SMCLK/4) pocitadlo pro zmenu stavu.
	 signal swap : std_logic := '0';	-- Enable signal pro zmenu stavu.

begin
-- Citac
	 prescaler: process(SMCLK, RESET)
	 begin
		if RESET = '1' then	-- Vynulovani citace.
			counter <= (others => '0');
			swap_counter <= (others => '0');
		elsif rising_edge(SMCLK) then	-- S kazdou nabeznou hranou zvysujeme pocitadla.
			counter <= counter + 1;
			if counter = "111000010000" then	-- Omezeni pocitadla
				counter <= (others => '0');
			end if;
			swap_counter <= swap_counter + 1;
			if swap_counter = "111000010000000000000" then	-- Omezeni pocitadla pro stavy.
				swap_counter <= (others => '0');
			end if;
		end if;
	end process prescaler;
		
-- Enable = 1, pokud citac dosahne pozadovane hodnoty.
	clk_enable <= '1' when counter = "111000010000" else '0';
	swap <= '1' when swap_counter = "111000010000000000000" else '0';

-- Rotacni registr
	row_rotation: process(SMCLK, RESET, rows)
	begin
		if RESET = '1' then
			rows <= "00000001";	-- Pocatecni radek = 1.
		elsif rising_edge(SMCLK) and clk_enable = '1' then
			rows <= rows(0) & rows(7 downto 1); -- Posunuti radku.
		end if;
	end process row_rotation;
	
-- Zmena stavu
	switch_state: process(swap)
	begin
		if rising_edge(swap) then
			case state is	-- Zmena stavu na zaklade predchoziho stavu.
				when "00" => state <= "10";
				when "10" => state <= "11";
				when "11" => state <= "01";
				when "01" => state <= "00";
				when others => state <= "00";
			end case;
		end if;
	end process switch_state;
		
-- Dekoder
	decoder: process(state, rows)
	begin
		if state = "00" or state = "11" then
			case rows is	-- Ledky v log. '1' jsou vypnute.
				when "10000000" => cols <= (others => '1');
				when "01000000" => cols <= (others => '1');
				when "00100000" => cols <= (others => '1');
				when "00010000" => cols <= (others => '1');
				when "00001000" => cols <= (others => '1');
				when "00000100" => cols <= (others => '1');
				when "00000010" => cols <= (others => '1');
				when "00000001" => cols <= (others => '1');
				when others => cols <= (others => '1');
			end case;
		elsif state = "10" then
				case rows is	-- Stav inicialu 'J'.
				when "10000000" => cols <= "11000011";
				when "01000000" => cols <= "10000001";
				when "00100000" => cols <= "10011001";
				when "00010000" => cols <= "10011111";
				when "00001000" => cols <= "10011111";
				when "00000100" => cols <= "10011111";
				when "00000010" => cols <= "10011111";
				when "00000001" => cols <= "10011111";
				when others => cols <= (others => '1');
			end case;
		elsif state = "01" then
				case rows is	-- Stav inicialu 'S'.
				when "10000000" => cols <= "11000011";
				when "01000000" => cols <= "10000001";
				when "00100000" => cols <= "10011111";
				when "00010000" => cols <= "10000011";
				when "00001000" => cols <= "11000001";
				when "00000100" => cols <= "11111001";
				when "00000010" => cols <= "10000001";
				when "00000001" => cols <= "11000011";
				when others => cols <= (others => '1');
			end case;
		end if;
	end process decoder;
	
-- Zobrazeni
	ROW <= rows;
	LED <= cols;
	
end main;
