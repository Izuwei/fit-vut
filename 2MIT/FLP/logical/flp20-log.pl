% Rubikova kostka

% Autor: Jakub Sadilek
% Login: xsadil07
% Datum: 2.5.2021


/**
 *                      Reprezentace kostky
 * 
 * +---------+                                         + --------- +
 * |         |                                        /           /|  3
 * |    5    |                                   4   /     5     / |
 * |         |                                      /           /  |
 * + ------- + ------- + ------- + ------- +       + --------- + 2 +
 * |         |         |         |         |       |           |  /
 * |    1    |    2    |    3    |    4    |       |     1     | /
 * |         |         |         |         |       |           |/
 * + ------- + ------- + ------- + ------- +       + --------- +
 * |         |                                           6
 * |    6    |
 * |         |
 * + ------- +
 * 
*/

/**
 * Nasleduji vsechny mozne rotace kostky v obou smerech.
*/
% Rotace kostky: vertikalne leva rada nahoru.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C11,C52,C53,
        C14,C55,C56,
        C17,C58,C59,

        C61,C12,C13,    C21,C22,C23,    C31,C32,C57,    C43,C46,C49,
        C64,C15,C16,    C24,C25,C26,    C34,C35,C54,    C42,C45,C48,
        C67,C18,C19,    C27,C28,C29,    C37,C38,C51,    C41,C44,C47,

        C39,C62,C63,
        C36,C65,C66,
        C33,C68,C69
    ]
).

% Rotace kostky: vertikalne leva rada dolu.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C39,C52,C53,
        C36,C55,C56,
        C33,C58,C59,

        C51,C12,C13,    C21,C22,C23,    C31,C32,C67,    C47,C44,C41,
        C54,C15,C16,    C24,C25,C26,    C34,C35,C64,    C48,C45,C42,
        C57,C18,C19,    C27,C28,C29,    C37,C38,C61,    C49,C46,C43,

        C11,C62,C63,
        C14,C65,C66,
        C17,C68,C69
    ]
).

% Rotace kostky: vertikalne prostredni rada nahoru.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C51,C12,C53,
        C54,C15,C56,
        C57,C18,C59,

        C11,C62,C13,    C21,C22,C23,    C31,C58,C33,    C41,C42,C43,
        C14,C65,C16,    C24,C25,C26,    C34,C55,C36,    C44,C45,C46,
        C17,C68,C19,    C27,C28,C29,    C37,C52,C39,    C47,C48,C49,

        C61,C38,C63,
        C64,C35,C66,
        C67,C32,C69
    ]
).

% Rotace kostky: vertikalne prostredni rada dolu.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C51,C38,C53,
        C54,C35,C56,
        C57,C32,C59,

        C11,C52,C13,    C21,C22,C23,    C31,C68,C33,    C41,C42,C43,
        C14,C55,C16,    C24,C25,C26,    C34,C65,C36,    C44,C45,C46,
        C17,C58,C19,    C27,C28,C29,    C37,C62,C39,    C47,C48,C49,

        C61,C12,C63,
        C64,C15,C66,
        C67,C18,C69
    ]
).

% Rotace kostky: vertikalne prava rada nahoru.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C51,C52,C13,
        C54,C55,C16,
        C57,C58,C19,

        C11,C12,C63,    C27,C24,C21,    C59,C32,C33,    C41,C42,C43,
        C14,C15,C66,    C28,C25,C22,    C56,C35,C36,    C44,C45,C46,
        C17,C18,C69,    C29,C26,C23,    C53,C38,C39,    C47,C48,C49,

        C61,C62,C37,
        C64,C65,C34,
        C67,C68,C31
    ]
).

% Rotace kostky: vertikalne prava rada dolu.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C51,C52,C37,
        C54,C55,C34,
        C57,C58,C31,

        C11,C12,C53,    C23,C26,C29,    C69,C32,C33,    C41,C42,C43,
        C14,C15,C56,    C22,C25,C28,    C66,C35,C36,    C44,C45,C46,
        C17,C18,C59,    C21,C24,C27,    C63,C38,C39,    C47,C48,C49,

        C61,C62,C13,
        C64,C65,C16,
        C67,C68,C19
    ]
).

% Rotace kostky: horizontalne horni rada doprava.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C53,C56,C59,
        C52,C55,C58,
        C51,C54,C57,

        C41,C42,C43,    C11,C12,C13,    C21,C22,C23,    C31,C32,C33,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ]
).

% Rotace kostky: horizontalne horni rada doleva.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C57,C54,C51,
        C58,C55,C52,
        C59,C56,C53,

        C21,C22,C23,    C31,C32,C33,    C41,C42,C43,    C11,C12,C13,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ]
).

% Rotace kostky: horizontalne prostredni rada doprava.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C44,C45,C46,    C14,C15,C16,    C24,C25,C26,    C34,C35,C36,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ]
).

% Rotace kostky: horizontalne prostredni rada doleva.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C24,C25,C26,    C34,C35,C36,    C44,C45,C46,    C14,C15,C16,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ]
).

% Rotace kostky: horizontalne dolni rada doprava.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C47,C48,C49,    C17,C18,C19,    C27,C28,C29,    C37,C38,C39,

        C67,C64,C61,
        C68,C65,C62,
        C69,C66,C63
    ]
).

% Rotace kostky: horizontalne dolni rada doleva.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C27,C28,C29,    C37,C38,C39,    C47,C48,C49,    C17,C18,C19,

        C63,C66,C69,
        C62,C65,C68,
        C61,C64,C67
    ]
).

% Rotace kostky: vertikalne leva rada nahoru pri pohledu z bocni prave strany.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C51,C52,C53,
        C54,C55,C56,
        C21,C24,C27,

        C13,C16,C19,    C63,C22,C23,    C31,C32,C33,    C41,C42,C59,
        C12,C15,C18,    C62,C25,C26,    C34,C35,C36,    C44,C45,C58,
        C11,C14,C17,    C61,C28,C29,    C37,C38,C39,    C47,C48,C57,

        C43,C46,C49,
        C64,C65,C66,
        C67,C68,C69
    ]
).

% Rotace kostky: vertikalne leva rada dolu pri pohledu z bocni prave strany.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C51,C52,C53,
        C54,C55,C56,
        C49,C46,C43,

        C17,C14,C11,    C57,C22,C23,    C31,C32,C33,    C41,C42,C61,
        C18,C15,C12,    C58,C25,C26,    C34,C35,C36,    C44,C45,C62,
        C19,C16,C13,    C59,C28,C29,    C37,C38,C39,    C47,C48,C63,

        C27,C24,C21,
        C64,C65,C66,
        C67,C68,C69
    ]
).

% Rotace kostky: vertikalne prostredni rada nahoru pri pohledu z bocni prave strany.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C51,C52,C53,
        C22,C25,C28,
        C57,C58,C59,

        C11,C12,C13,    C21,C66,C23,    C31,C32,C33,    C41,C56,C43,
        C14,C15,C16,    C24,C65,C26,    C34,C35,C36,    C44,C55,C46,
        C17,C18,C19,    C27,C64,C29,    C37,C38,C39,    C47,C54,C49,

        C61,C62,C63,
        C42,C45,C48,
        C67,C68,C69
    ]
).

% Rotace kostky: vertikalne prostredni rada dolu pri pohledu z bocni prave strany.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C51,C52,C53,
        C48,C45,C42,
        C57,C58,C59,

        C11,C12,C13,    C21,C54,C23,    C31,C32,C33,    C41,C64,C43,
        C14,C15,C16,    C24,C55,C26,    C34,C35,C36,    C44,C65,C46,
        C17,C18,C19,    C27,C56,C29,    C37,C38,C39,    C47,C66,C49,

        C61,C62,C63,
        C28,C25,C22,
        C67,C68,C69
    ]
).

% Rotace kostky: vertikalne prava rada nahoru pri pohledu z bocni prave strany.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C23,C26,C29,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C69,    C37,C34,C31,    C53,C42,C43,
        C14,C15,C16,    C24,C25,C68,    C38,C35,C32,    C52,C45,C46,
        C17,C18,C19,    C27,C28,C67,    C39,C36,C33,    C51,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C41,C44,C47
    ]
).

% Rotace kostky: vertikalne prava rada dolu pri pohledu z bocni prave strany.
rotate_cube(
    [
        C51,C52,C53,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C23,    C31,C32,C33,    C41,C42,C43,
        C14,C15,C16,    C24,C25,C26,    C34,C35,C36,    C44,C45,C46,
        C17,C18,C19,    C27,C28,C29,    C37,C38,C39,    C47,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C67,C68,C69
    ],
    [
        C47,C44,C41,
        C54,C55,C56,
        C57,C58,C59,

        C11,C12,C13,    C21,C22,C51,    C33,C36,C39,    C67,C42,C43,
        C14,C15,C16,    C24,C25,C52,    C32,C35,C38,    C68,C45,C46,
        C17,C18,C19,    C27,C28,C53,    C31,C34,C37,    C69,C48,C49,

        C61,C62,C63,
        C64,C65,C66,
        C29,C26,C23
    ]
).

% Vyresena kostka -> strany obsahuji stejne znaky.
is_solved(
    [
        C5,C5,C5,
        C5,C5,C5,
        C5,C5,C5,

        C1,C1,C1,    C2,C2,C2,    C3,C3,C3,    C4,C4,C4,
        C1,C1,C1,    C2,C2,C2,    C3,C3,C3,    C4,C4,C4,
        C1,C1,C1,    C2,C2,C2,    C3,C3,C3,    C4,C4,C4,

        C6,C6,C6,
        C6,C6,C6,
        C6,C6,C6
    ]
).


/** 
 * Nasledujici cast kodu je prevzata z prilozene ukazky input2.pl.
*/
% Cte radky ze standardniho vstupu, konci na LF nebo EOF.
read_line(L,C) :-
	get_char(C),
	(isEOFEOL(C), L = [], !;
		read_line(LL,_),% atom_codes(C,[Cd]),
		[C|LL] = L).

% Testuje znak na EOF nebo LF.
isEOFEOL(C) :-
	C == end_of_file;
	(char_code(C,Code), Code==10).

read_lines(Ls) :-
	read_line(L,C),
	( C == end_of_file, Ls = [] ;
	  read_lines(LLs), Ls = [L|LLs]
	).
    
% Rozdeli radek na podseznamy.
split_line([],[[]]) :- !.
split_line([' '|T], [[]|S1]) :- !, split_line(T,S1).
split_line([32|T], [[]|S1]) :- !, split_line(T,S1).    % aby to fungovalo i s retezcem na miste seznamu
split_line([H|T], [[H|G]|S1]) :- split_line(T,[G|S1]). % G je prvni seznam ze seznamu seznamu G|S1

% Vstupem je seznam radku (kazdy radek je seznam znaku).
split_lines([],[]).
split_lines([L|Ls],[H|T]) :- split_lines(Ls,T), split_line(L,H).
/**
 * Konec prevzate casti.
*/


% Rekurzivne aplikuje vsechny mozne rotace kostky az najde slozenou kostku.
solve_cube(Cube, Cube, []) :- is_solved(Cube).
solve_cube(Cube, SolvedCube, [NewCube | Cubes]) :- solve_cube(NewCube, SolvedCube, Cubes), rotate_cube(Cube, NewCube).

% Vypise N prvku z listu.
show_n(_, [], []).
show_n(0, Rest, Rest).
show_n(N, [Cell | Cells], Rest) :- write(Cell), NewN is N-1, show_n(NewN, Cells, Rest).

% Vypise zadanou kostku (jak je definovano v zadani).
show_cube(Cube) :- 
    show_n(3, Cube,R01), nl,
    show_n(3, R01, R02), nl,
    show_n(3, R02, R03), nl,
    show_n(3, R03, R04), write(' '), show_n(3, R04, R05), write(' '), show_n(3, R05, R06), write(' '), show_n(3, R06, R07), nl,
    show_n(3, R07, R08), write(' '), show_n(3, R08, R09), write(' '), show_n(3, R09, R10), write(' '), show_n(3, R10, R11), nl,
    show_n(3, R11, R12), write(' '), show_n(3, R12, R13), write(' '), show_n(3, R13, R14), write(' '), show_n(3, R14, R15), nl,
    show_n(3, R15, R16), nl,
    show_n(3, R16, R17), nl,
    show_n(3, R17, _),   nl.

% Vypise vsechny kostky ze seznamu.
show_cubes([]).
show_cubes([Cube | Cubes]) :- nl, show_cube(Cube), show_cubes(Cubes).


% Program zacne zde.
start :-
    prompt(_, ''),
    read_lines(Lines),
    split_lines(Lines, SplitedLines),
    flatten(SplitedLines, Cube),    % Zjednodusi list

    solve_cube(Cube, _, History),   % Vyresi kostku
    
    show_cube(Cube),                % Vypise pocatecni kostku
    show_cubes(History),            % Vypise historii rotaci kostky

    halt.
