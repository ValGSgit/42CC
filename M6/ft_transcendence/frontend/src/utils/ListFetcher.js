import api from "../services/api.js";

class ListFetcher {
  constructor(
    pageSize = 10,
    page = 1,
    filter = {},
    sort = {},
    additionalParams = {},
  ) {
    this.pageSize = pageSize;
    this.page = page;
    this.filter = filter;
    this.sort = sort;
    this.additionalParams = additionalParams;

    this.total = 0;

    this.loading = false;
  }

  updateParams(params = {}) {
    if (params.pageSize !== undefined) this.pageSize = params.pageSize;
    if (params.page !== undefined) this.page = params.page;
    if (params.filter !== undefined) {
      this.filter = params.filter;
      this.page = 1;
    }
    if (params.sort !== undefined) {
      this.sort = params.sort;
    }
    if (params.additionalParams !== undefined)
      this.additionalParams = { ...params.additionalParams };
  }

  nextPage() {
    this.page += 1;
  }

  previousPage() {
    this.page = Math.max(1, this.page - 1);
  }

  isFirstPage() {
    return this.page === 1;
  }

  isLastPage() {
    return this.page * this.pageSize >= this.total;
  }

  multiplePages() {
    return this.total > this.pageSize;
  }

  async fetch(baseUrl) {
    try {
      this.loading = true;
      let params = { ...this.additionalParams };

      params.limit = this.pageSize;
      params.offset = (this.page - 1) * this.pageSize;

      if (this.filter !== "") {
        for (const [key, value] of Object.entries(this.filter)) {
          if (value === undefined || value === null || value === "") continue;
          params[`filter[${key}]`] = value;
        }
      }

      if (this.sort !== "") {
        for (const [key, value] of Object.entries(this.sort)) {
          if (value !== "asc" && value !== "desc") continue;
          params[`sort[${key}]`] = value;
        }
      }

      const response = await api.get(baseUrl, { params });
      this.total = response.data.total ?? 0;
      this.loading = false;
      return response;
    } catch (error) {
      this.total = 0;
      this.loading = false;
      throw error;
    }
  }
}

export default ListFetcher;
