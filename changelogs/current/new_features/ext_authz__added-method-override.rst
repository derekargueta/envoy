Added :ref:`method_override
<envoy_v3_api_field_extensions.filters.http.ext_authz.v3.HttpService.method_override>`
to the HTTP ext_authz filter. When set, the HTTP method of the request sent to the authorization
service is replaced with this value, regardless of the original request method. This allows
the authorization service to expose a single fixed endpoint (e.g. ``POST /auth``) rather than
handling every method the upstream receives.
