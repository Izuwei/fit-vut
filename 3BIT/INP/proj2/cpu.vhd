-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2018 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Jakub Sadilek
--	Login: xsadil07

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal 
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni z pameti (DATA_RDWR='1') / zapis do pameti (DATA_RDWR='0')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WE musi byt '0'
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 -- Programovy citac (PC):
	signal pc_inc	:	std_logic;
	signal pc_dec	:	std_logic;
	signal pc_reg	:	std_logic_vector(11 downto 0) := (others => '0');
	
 -- Citac zavorek ve whilu (CNT):
	signal cnt_inc	:	std_logic;
	signal cnt_dec	:	std_logic;
	signal cnt_reg	:	std_logic_vector(7 downto 0) := (others => '0');
	
 -- Ukazatel do pameti (PTR):
	signal ptr_inc	:	std_logic;
	signal ptr_dec	:	std_logic;
	signal ptr_reg	:	std_logic_vector(9 downto 0) := (others => '0');
	
 -- Multiplexor (MX)
	signal mx_sel	:	std_logic_vector(1 downto 0);
	
	signal shift_reg	:	std_logic_vector(7 downto 0) := (others => '0');
	
 -- Konecny automat (FSM)
	type fsm_state is (	-- stavy
		fetch,	-- Nacteni instrukce
		decode,	-- Dekodovani instrukce
		inc_ptr,	-- Inkrementace ukazatele do pameti
		dec_ptr,	-- Dekrementace ukazatele do pameti
		inc_data,	-- Inkrementace hodnoty bunky
		inc_data2,
		dec_data, 	-- Dekrementace hodnoty bunky
		dec_data2,
		beg_while,	-- Zacatek cyklu while
		beg_while2,
		beg_while3,
		beg_while4,
		end_while,	-- Konec cyklu while
		end_while2,
		end_while3,
		end_while4,
		end_while5,
		print_data,	-- Vytiskneme data na vystup
		print_data2,
		load_data,	-- Nacteni hodnoty a ulozeni do bunky
		load_data2,
		com,		-- Komentar
		com2,
		com3,
		hexa,		-- Hexadecimalni cislo
		hexa2,
		hexa3,
		stop,		-- Zastaveni programu
		other		-- Jine znaky
	);
	signal state		:	fsm_state;	-- Aktualni stav
	signal next_state :	fsm_state;	-- Pristi stav

begin

 -- Programovy citac (PC):
	pc: process (CLK, RESET, pc_reg, pc_inc, pc_dec)
		begin
			if RESET = '1' then			-- Pokud RESET, tak vynulujeme citac.
				pc_reg <= (others => '0');
			elsif rising_edge(CLK) then
				if pc_inc = '1' then		-- Pokud inc je v log. 1, tak inkrementujeme.
					pc_reg <= pc_reg + 1;
				elsif pc_dec = '1' then	-- Pokud dec je v log. 1,tak dekrementujeme.
					pc_reg <= pc_reg - 1;
				end if;
			end if;
		end process;
	
	CODE_ADDR <= pc_reg;	-- pc_reg nasmerujeme na port CODE_ADDR
	
 -- Citac zavorek whilu (CNT):
	cnt: process (CLK, RESET, cnt_reg, cnt_inc, cnt_dec)
		begin
			if RESET = '1' then				-- Pokud RESET, tak vynulujeme citac.
				cnt_reg <= (others => '0');
			elsif rising_edge(CLK) then
				if cnt_inc = '1' then		-- Pokud inc, tak inkrementujeme.
					cnt_reg <= cnt_reg + 1;
				elsif cnt_dec = '1' then	-- Pokud dec, tak dekrementujeme.
					cnt_reg <= cnt_reg - 1;
				end if;
			end if;
		end process;
		
	
 -- Ukazatel do pameti (PTR):
	ptr: process (CLK, RESET, ptr_reg, ptr_inc, ptr_dec)
		begin
			if RESET = '1' then				-- Pokud RESET, tak nulujeme ukazatel.
				ptr_reg <= (others => '0');
			elsif rising_edge(CLK) then
				if ptr_inc = '1' then		-- Pokud inc, tak inkrementujeme uk.
					ptr_reg <= ptr_reg + 1;
				elsif ptr_dec = '1' then	-- Pokud dec, tak dekrementujeme uk.
					ptr_reg <= ptr_reg - 1;
				end if;
			end if;
		end process;
		
	DATA_ADDR <= ptr_reg;	-- ptr_reg nasmerujeme na port DATA_ADDR
	
 -- Multiplexor (MX):
	mx: process (IN_DATA, DATA_RDATA, mx_sel, shift_reg)
		begin
			case mx_sel is	-- Podle hodnoty sel, vybereme vstup na vystup.
				when "00" => DATA_WDATA <= IN_DATA;
				when "01" => DATA_WDATA <= shift_reg;
				when "10" => DATA_WDATA <= DATA_RDATA - 1;	-- Hodnoty podle schematu.
				when "11" => DATA_WDATA <= DATA_RDATA + 1;
				when others =>	-- Nemelo by nastat 
			end case;
		end process;
	
	OUT_DATA <= DATA_RDATA;	-- Nasmerovani dat z RAM na vystup.
	
 -- Konecny automat (FSM):
	-- Zmena stavu na nasledujici
	fsm_chng: process (CLK, RESET, EN, state, next_state)
		begin
			if RESET = '1' then	-- Pokud RESET, pocatecni stav je inicializace.
				state <= fetch;
			elsif rising_edge(CLK) then
				if EN = '1' then	-- Pokud CLK a zaroven EN, tak menime stav.
					state <= next_state;
				end if;
			end if;
		end process;
		
	-- Logika konecneho automatu
	fsm: process (OUT_BUSY, IN_VLD, CODE_DATA, DATA_RDATA, state, next_state, cnt_reg)
		begin
			next_state <= fetch;	-- Inicializace pocatecnich stavu
			OUT_WE <= '0';		-- Povoleni zapisu
			IN_REQ <= '0';		-- Pozadavek na data
			CODE_EN <= '1';	-- Povoleni cinosti ROM
			DATA_EN <= '0';	-- Cinost pameti RAM
			pc_inc <= '0';		-- Programovy citac inc/dec
			pc_dec <= '0';
			cnt_inc <= '0';	-- Counter inc/dec
			cnt_dec <= '0';
			ptr_inc <= '0';	-- Ukazatel do pameti inc/dec
			ptr_dec <= '0';
			mx_sel <= "00";	-- Multiplexor
			DATA_RDWR <= '0';	-- Cteni/Zapis do pameti RAM
		
			case state is
				when fetch =>	-- Nacteni instrukce
					CODE_EN <= '1';
					next_state <= decode;
					
				when decode =>	-- Dekodovani instrukce
					case CODE_DATA is
						when X"3E" => next_state <= inc_ptr;
						when X"3C" => next_state <= dec_ptr;
						when X"2B" => next_state <= inc_data;
						when X"2D" => next_state <= dec_data;
						when X"5B" => next_state <= beg_while;
						when X"5D" => next_state <= end_while;
						when X"2E" => next_state <= print_data;
						when X"2C" => next_state <= load_data;
						when X"23" => next_state <= com;
						when X"30" | X"31" | X"32" | X"33" | 
							  X"34" | X"35" | X"36" | X"37" |
							  X"38" | X"39" | X"41" | X"42" |
							  X"43" | X"44" | X"45" | X"46" => next_state <= hexa;
						when X"00" => next_state <= stop;
						when others => next_state <= other;
					end case;
					
				when inc_ptr =>	--	Inkrementace ukazatele
					pc_inc <= '1';
					ptr_inc <= '1';
					next_state <= fetch;
					
				when dec_ptr =>	--	Dekrementace ukazatele
					pc_inc <= '1';
					ptr_dec <= '1';
					next_state <= fetch;
					
				when inc_data =>	-- Inkrementace hodnoty
					pc_inc <= '1';
					DATA_EN <= '1';
					DATA_RDWR <= '1';	-- Nacteni dat
					next_state <= inc_data2;
					
				when inc_data2 =>
					DATA_EN <= '1';
					DATA_RDWR <= '0';	-- Zapis inkrementovanych dat
					mx_sel <= "11";	-- DATA_RDATA + 1
					next_state <= fetch;
			
				when dec_data =>	-- Dekrementace hodnoty
					pc_inc <= '1';
					DATA_EN <= '1';
					DATA_RDWR <= '1'; -- Precteme hodnotu
					next_state <= dec_data2;
					
				when dec_data2 =>
					DATA_EN <= '1';
					DATA_RDWR <= '0';	-- Zapiseme hodnotu
					mx_sel <= "10";	-- DATA_RDATA - 1
					next_state <= fetch;
										
				when print_data =>	--	Vytiskneme data
					pc_inc <= '1';
					DATA_EN <= '1';
					DATA_RDWR <= '1';	-- Precteme data
					next_state <= print_data2;
					
				when print_data2 =>
					if OUT_BUSY = '1' then	-- Cekame na volny vystup
						next_state <= print_data2;
					else	-- Posleme data na vystup
						OUT_WE <= '1';
						next_state <= fetch;		
					end if;
				
				when load_data =>		-- Nacteme data ze vstupu
					IN_REQ <= '1';
					mx_sel <= "00";	-- Nastavime mux na vstup
					next_state <= load_data2;
					
				when load_data2 =>
					if IN_VLD /= '1' then	-- Validni vstup
						next_state <= load_data2;
					else
						pc_inc <= '1';
						DATA_EN <= '1';
						DATA_RDWR <= '0';	-- Zapiseme data
						next_state <= fetch;
					end if;
					
				when com =>		-- Komentar
					pc_inc <= '1';
					next_state <= com2;
				
				when com2 =>
					-- pc_inc <= '1';
					CODE_EN <= '1';
					next_state <= com3;
					
				when com3 =>
					if CODE_DATA = X"23" then
						pc_inc <= '1';	-- Ukonceny komentar
						next_state <= fetch;
					else		-- Jdeme na dalsi znak dokud nebude #
						next_state <= com;
					end if;

				when hexa =>	-- Hexadecimalni cislo
					pc_inc <= '1';
					if CODE_DATA < X"40" then
						next_state <= hexa2;	-- 0-9
					else
						next_state <= hexa3;	-- A-F
					end if;
					
				when hexa2 =>	-- Hexa 0-9
					DATA_EN <= '1';
					mx_sel <= "01";	-- Jakože shift doleva o 4 bity
					shift_reg <= CODE_DATA(3 downto 0) & "0000";
					next_state <= fetch;
					
				when hexa3 =>	-- Hexa A-F
					DATA_EN <= '1';
					mx_sel <= "01";	-- To stejne jako u hexa2 s korekci.
					shift_reg <= (CODE_DATA(3 downto 0) + 9) & "0000";
					next_state <= fetch;
				
				when beg_while =>		-- Zacatek cyklu while
					pc_inc <= '1';
					DATA_EN <= '1';
					DATA_RDWR <= '1';	-- Precteme hodnotu bunky
					next_state <= beg_while2;
					
				when beg_while2 => -- Rozhodneme podle hodnoty vykonavani
					if DATA_RDATA /= (DATA_RDATA'range => '0') then
						next_state <= fetch;	-- Jsme v cyklu, bunka != 0
					else	-- Bunka je 0 -> preskocime while
						cnt_inc <= '1';
						next_state <= beg_while3;
					end if;
					
				when beg_while3 =>	-- Pocitame zavorky dokud neni 0
					if cnt_reg = (cnt_reg'range => '0') then
						next_state <= fetch;	-- Kontrola ukonceni whilu
					else
						pc_inc <= '1';
						CODE_EN <= '1';	-- Aktualizace CODE_DATA
						next_state <= beg_while4;
					end if;
					
				when beg_while4 =>
					if CODE_DATA = X"5B" then	-- [ (inkrementujeme zavorky)
						cnt_inc <= '1';	-- Dalsi while (vnoreny)
					elsif CODE_DATA = X"5D" then -- ] (dekrementujeme zavorky)
						cnt_dec <= '1'; 	-- Ukonceni whilu
					end if;
					next_state <= beg_while3;
					
				when end_while =>		-- Ukonceni whilu
					DATA_EN <= '1';	-- DATA_RDATA = RAM[PTR]
					DATA_RDWR <= '1';
					next_state <= end_while2;
					
				when end_while2 =>	-- Rozhodneme ukonceni whilu
					if DATA_RDATA /= (DATA_RDATA'range => '0') then
						cnt_inc <= '1';	-- Cyklime
						pc_dec <= '1';
						next_state <= end_while3;
					else	-- Konec whilu
						pc_inc <= '1';
						next_state <= fetch;
					end if;
					
				when end_while3 =>
					CODE_EN <= '1';
					next_state <= end_while4;
					
				when end_while4 =>	-- Kontrola zavorek
					if cnt_reg = (cnt_reg'range => '0') then
						next_state <= fetch; -- Pokracujeme
					else
						if CODE_DATA = X"5B" then	-- [
							cnt_dec <= '1';	-- Zacatek dekrementujeme
						elsif CODE_DATA = X"5D" then	-- ]
							cnt_inc <= '1';	-- Konec inkrementujeme
						end if;
						next_state <= end_while5;
					end if;
				
				when end_while5 =>	-- Smer
					if cnt_reg = (cnt_reg'range => '0') then
						pc_inc <= '1';	-- Jsme na zacatku -> jdem znova
					else
						pc_dec <= '1';	-- Nejsme na zacatku -> jdem dal
					end if;
					next_state <= end_while3;
				
				when stop =>	-- Zastavime vykonavani programu
					next_state <= stop;
					
				when other =>	-- Ostatni
					pc_inc <= '1';
					next_state <= fetch;
					
				when others => null;	-- Nemelo by nastat
							
			end case;
		end process;

end behavioral;
 