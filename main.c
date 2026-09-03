#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define CANDY_KINDS  6U
#define BASKET_MAX   8U
#define NAME_LEN     16U

typedef struct {
    char     name[NAME_LEN];
    uint16_t price;      /* piastres */
    uint16_t stock;      /* how many are left on the shelf */
    uint16_t sold;       /* how many we sold today */
} Candy_t;

typedef struct {
    uint8_t  candyId;
    uint8_t  qty;
} Line_t;

static Candy_t shelf[CANDY_KINDS];
static Line_t  basket[BASKET_MAX];
static uint8_t basketLines;      /* how many slots of the basket are used */
static uint32_t cashDrawer;

static void clearBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

static void openShop(void) {
    const char *names[CANDY_KINDS] = {"Gummy Bear", "Chocolate", "Lollipop", "Jelly Bean", "Caramel", "Licorice"};
    const uint16_t prices[CANDY_KINDS] = {50U, 150U, 75U, 25U, 100U, 60U};
    const uint16_t stocks[CANDY_KINDS] = {20U, 10U, 15U, 30U, 12U, 18U};

    for (uint8_t i = 0U; i < CANDY_KINDS; ++i) {
        strncpy(shelf[i].name, names[i], NAME_LEN);
        shelf[i].price = prices[i];
        shelf[i].stock = stocks[i];
        shelf[i].sold = 0U;
    }
    basketLines = 0U;
    cashDrawer = 0U;
}

static void showShelf(void) {
    printf("\n--- Candy Shelf ---\n");
    printf("%-3s | %-12s | %-6s | %-8s\n", "No.", "Name", "Price", "Stock");
    printf("------------------------------------\n");
    for (uint8_t i = 0U; i < CANDY_KINDS; ++i) {
        printf("%-3u | %-12s | %-4u pt | ", i, shelf[i].name, shelf[i].price);
        if (shelf[i].stock == 0U) {
            printf("SOLD OUT\n");
        } else {
            printf("%-8u\n", shelf[i].stock);
        }
    }
}

static void addToBasket(void) {
    int id = 0;
    int qty = 0;

    showShelf();
    printf("Enter candy number (0-%u): ", CANDY_KINDS - 1);
    if (scanf("%d", &id) != 1 || id < 0 || (uint8_t)id >= CANDY_KINDS) {
        printf("Invalid candy number!\n");
        clearBuffer();
        return;
    }

    if (shelf[id].stock == 0U) {
        printf("Sorry, this candy is SOLD OUT!\n");
        return;
    }

    printf("Enter quantity: ");
    if (scanf("%d", &qty) != 1 || qty <= 0) {
        printf("Invalid quantity!\n");
        clearBuffer();
        return;
    }

    if ((uint16_t)qty > shelf[id].stock) {
        printf("Not enough stock available! Only %u left.\n", shelf[id].stock);
        return;
    }

    /* Check if already in basket to merge lines */
    int existingLine = -1;
    for (uint8_t i = 0U; i < basketLines; ++i) {
        if (basket[i].candyId == (uint8_t)id) {
            existingLine = (int)i;
            break;
        }
    }

    if (existingLine != -1) {
        uint32_t newQty = (uint32_t)basket[existingLine].qty + (uint32_t)qty;
        if (newQty > shelf[id].stock) {
            printf("Total quantity exceeds available stock!\n");
            return;
        }
        basket[existingLine].qty = (uint8_t)newQty;
    } else {
        if (basketLines >= BASKET_MAX) {
            printf("Basket is full!\n");
            return;
        }
        basket[basketLines].candyId = (uint8_t)id;
        basket[basketLines].qty = (uint8_t)qty;
        basketLines++;
    }
    printf("Added to basket successfully.\n");
}

static void removeFromBasket(void) {
    int line = 0;
    if (basketLines == 0U) {
        printf("Basket is empty!\n");
        return;
    }

    printf("\n--- Current Basket Lines ---\n");
    for (uint8_t i = 0U; i < basketLines; ++i) {
        printf("[%u] %s (Qty: %u)\n", i, shelf[basket[i].candyId].name, basket[i].qty);
    }
    printf("Enter line number to remove: ");
    if (scanf("%d", &line) != 1 || line < 0 || (uint8_t)line >= basketLines) {
        printf("Invalid line number!\n");
        clearBuffer();
        return;
    }

    /* Slide lines up */
    for (uint8_t i = (uint8_t)line; i < basketLines - 1U; ++i) {
        basket[i] = basket[i + 1U];
    }
    basketLines--;
    printf("Line removed from basket.\n");
}

static uint32_t basketTotal(void) {
    uint32_t total = 0U;
    for (uint8_t i = 0U; i < basketLines; ++i) {
        uint8_t id = basket[i].candyId;
        total += (uint32_t)basket[i].qty * (uint32_t)shelf[id].price;
    }
    return total;
}

static void showBasket(void) {
    if (basketLines == 0U) {
        printf("\nBasket is empty.\n");
        return;
    }
    printf("\n============= RECEIPT =============\n");
    for (uint8_t i = 0U; i < basketLines; ++i) {
        uint8_t id = basket[i].candyId;
        uint32_t lineCost = (uint32_t)basket[i].qty * (uint32_t)shelf[id].price;
        printf("%-12s x%-3u @ %-3u pt = %u pt\n",
               shelf[id].name, basket[i].qty, shelf[id].price, lineCost);
    }
    printf("-----------------------------------\n");
    printf("Total Basket Cost: %u piastres\n", basketTotal());
    printf("===================================\n");
}

static void giveChange(uint32_t change) {
    if (change == 0U) {
        printf("No change, thank you!\n");
        return;
    }
    printf("Change to give: %u piastres\n", change);
    
    uint16_t coins[] = {500U, 200U, 100U, 50U, 25U};
    for (uint8_t i = 0U; i < 5U; ++i) {
        if (change >= coins[i]) {
            uint32_t count = change / coins[i];
            change %= coins[i];
            printf("- %u x %u pt coins\n", count, coins[i]);
        }
    }
    if (change > 0U) {
        printf("- Reminder: %u piastres remaining could not be returned in exact standard coins (fractional/remainder).\n", change);
    }
}

static void checkout(void) {
    uint32_t total = basketTotal();
    uint32_t paid = 0U;

    if (basketLines == 0U) {
        printf("Basket is empty, nothing to checkout!\n");
        return;
    }

    showBasket();
    printf("Enter money handed over (in piastres): ");
    if (scanf("%u", &paid) != 1) {
        printf("Invalid input!\n");
        clearBuffer();
        return;
    }

    if (paid < total) {
        printf("Not enough money! You need %u pt more. Basket remains untouched.\n", total - paid);
        return;
    }

    /* Process checkout: update shelf stock, sold counts, cash drawer */
    for (uint8_t i = 0U; i < basketLines; ++i) {
        uint8_t id = basket[i].candyId;
        uint8_t qty = basket[i].qty;
        shelf[id].stock -= qty;
        shelf[id].sold += qty;
    }

    cashDrawer += total;
    uint32_t change = paid - total;
    printf("Payment accepted! Thank you.\n");
    giveChange(change);

    /* Empty basket */
    basketLines = 0U;
}

static uint8_t bestSeller(void) {
    uint8_t best = 0U;
    for (uint8_t i = 1U; i < CANDY_KINDS; ++i) {
        if (shelf[i].sold > shelf[best].sold) {
            best = i;
        }
    }
    return best;
}

static void dayReport(void) {
    uint32_t totalSoldAll = 0U;
    uint8_t best = bestSeller();

    for (uint8_t i = 0U; i < CANDY_KINDS; ++i) {
        totalSoldAll += shelf[i].sold;
    }

    printf("\n================ DAY REPORT ================\n");
    printf("Cash in Drawer   : %u piastres\n", cashDrawer);
    printf("Total Candies Sold: %u\n", totalSoldAll);
    if (totalSoldAll > 0U) {
        printf("Best Seller      : %s (%u sold)\n", shelf[best].name, shelf[best].sold);
    } else {
        printf("Best Seller      : None yet\n");
    }
    printf("--- Sold Out Items ---\n");
    uint8_t soldOutCount = 0U;
    for (uint8_t i = 0U; i < CANDY_KINDS; ++i) {
        if (shelf[i].stock == 0U) {
            printf("- %s\n", shelf[i].name);
            soldOutCount++;
        }
    }
    if (soldOutCount == 0U) {
        printf("None (All items still have stock).\n");
    }
    printf("============================================\n");
}

int main(void) {
    int option = 0;
    openShop();

    do {
        printf("\n--- Candy Shop Menu ---\n");
        printf("1. Show Shelf\n");
        printf("2. Add to Basket\n");
        printf("3. Remove from Basket\n");
        printf("4. Show Basket\n");
        printf("5. Checkout\n");
        printf("6. Day Report\n");
        printf("7. Reset/Open Shop\n");
        printf("0. Exit\n");
        printf("Choice: ");

        if (scanf("%d", &option) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clearBuffer();
            continue;
        }

        switch (option) {
            case 1: showShelf(); break;
            case 2: addToBasket(); break;
            case 3: removeFromBasket(); break;
            case 4: showBasket(); break;
            case 5: checkout(); break;
            case 6: dayReport(); break;
            case 7: openShop(); printf("Shop reset/opened fresh.\n"); break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Unknown option!\n"); break;
        }
    } while (option != 0);

    return 0;
}